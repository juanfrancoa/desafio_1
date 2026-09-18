#include "tablerobits.h"
#include "logicajuego.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

const unsigned int DIMENSION_MAXIMA = 99;

// Lee un entero en [minimo, maximo]; repite ante entradas invalidas.
// Si la entrada se cierra devuelve 'minimo' (en el menu equivale a Salir).
static unsigned int leerEntero(const char* mensaje, unsigned int minimo, unsigned int maximo)
{
    unsigned int resultado = minimo;
    bool valido = false;
    while (!valido) {
        cout << mensaje << " [" << minimo << "-" << maximo << "]: ";
        unsigned int valor = 0;   // un negativo se lee como un valor enorme y queda fuera de rango
        if (cin >> valor && valor >= minimo && valor <= maximo) {
            resultado = valor;
            valido = true;
        } else if (cin.eof()) {
            valido = true;
        } else {
            cout << "Valor invalido.\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
    return resultado;
}

static void mostrarEstado(const unsigned char* tablero, unsigned int filas, unsigned int columnas,
                          unsigned int capacidad, const unsigned int* estadisticas)
{
    cout << "\n==================================================\n";
    imprimirTablero(tablero, filas, columnas);
    cout << "\nFichas: # $ % & * +   (. = vacio)\n"
         << "Tablero: " << filas << " x " << columnas
         << "  | Memoria: " << bytesNecesarios(filas, columnas) << " bytes usados de "
         << capacidad << " reservados\n"
         << "Eliminaciones del jugador: " << estadisticas[EST_ELIMINACIONES_USUARIO]
         << "  | Fichas eliminadas: "     << estadisticas[EST_FICHAS_ELIMINADAS] << '\n'
         << "Combinaciones: "             << estadisticas[EST_COMBINACIONES]
         << "  | Cascadas (ultima jugada): " << estadisticas[EST_CASCADAS_ACTUAL]
         << "  | Cascadas (total): "      << estadisticas[EST_CASCADAS_TOTAL] << '\n'
         << "PUNTOS: "                    << estadisticas[EST_PUNTOS] << '\n'
         << "==================================================\n";
}

int main()
{
    srand(time(nullptr));

    cout << "=== SWEET CRUSH ===\n";
    unsigned int filas    = leerEntero("Filas", 1, DIMENSION_MAXIMA);
    unsigned int columnas = leerEntero("Columnas", 1, DIMENSION_MAXIMA);

    unsigned char* tablero = crearTablero(filas, columnas);
    unsigned int capacidad = bytesNecesarios(filas, columnas);
    unsigned int estadisticas[NUM_ESTADISTICAS] = {0};

    // El tablero inicial no debe traer combinaciones hechas: se resuelven sin puntuar
    llenarAleatorio(tablero, filas, columnas);
    procesarCascadas(tablero, filas, columnas, nullptr, false);

    unsigned int opcion = 1;
    while (opcion != 0) {
        mostrarEstado(tablero, filas, columnas, capacidad, estadisticas);
        cout << "1. Eliminar ficha\n"
             << "2. Agregar fila\n"
             << "3. Eliminar fila\n"
             << "4. Agregar columna\n"
             << "5. Eliminar columna\n"
             << "6. Ver tablero en binario\n"
             << "7. Ver bytes en memoria\n"
             << "0. Salir\n";
        opcion = leerEntero("Opcion", 0, 7);

        switch (opcion) {
        case 1: {
            unsigned int f = leerEntero("Fila", 0, filas - 1);
            unsigned int c = leerEntero("Columna", 0, columnas - 1);
            eliminarFichaUsuario(tablero, filas, columnas, f, c, estadisticas);
            break;
        }
        case 2: {
            if (filas >= DIMENSION_MAXIMA) { cout << "Tamano maximo alcanzado.\n"; break; }
            unsigned int p = leerEntero("Insertar fila en la posicion", 0, filas);
            agregarFila(tablero, filas, columnas, p, capacidad);
            procesarCascadas(tablero, filas, columnas, estadisticas, true);
            break;
        }
        case 3: {
            if (filas <= 1) { cout << "El tablero debe tener al menos una fila.\n"; break; }
            unsigned int p = leerEntero("Fila a eliminar", 0, filas - 1);
            eliminarFila(tablero, filas, columnas, p, capacidad);
            procesarCascadas(tablero, filas, columnas, estadisticas, true);
            break;
        }
        case 4: {
            if (columnas >= DIMENSION_MAXIMA) { cout << "Tamano maximo alcanzado.\n"; break; }
            unsigned int p = leerEntero("Insertar columna en la posicion", 0, columnas);
            agregarColumna(tablero, filas, columnas, p, capacidad);
            procesarCascadas(tablero, filas, columnas, estadisticas, true);
            break;
        }
        case 5: {
            if (columnas <= 1) { cout << "El tablero debe tener al menos una columna.\n"; break; }
            unsigned int p = leerEntero("Columna a eliminar", 0, columnas - 1);
            eliminarColumna(tablero, filas, columnas, p, capacidad);
            procesarCascadas(tablero, filas, columnas, estadisticas, true);
            break;
        }
        case 6:
            cout << '\n';
            imprimirBinario(tablero, filas, columnas);
            break;
        case 7:
            cout << '\n';
            imprimirBytes(tablero, filas, columnas);
            break;
        default:
            break;
        }
    }

    liberarTablero(tablero);
    cout << "Puntaje final: " << estadisticas[EST_PUNTOS] << '\n';
    return 0;
}