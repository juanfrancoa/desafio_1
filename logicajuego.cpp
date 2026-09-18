#include "logicajuego.h"
#include "tablerobits.h"
#include <iostream>
using namespace std;

// ================================================================ Mascara de bits
// Un bit por posicion para marcar fichas a eliminar. Permite que una ficha
// que esta en una combinacion horizontal y en una vertical se marque dos
// veces sin eliminarse dos veces (marcar con | es idempotente).

static void marcarBit(unsigned char* mascara, unsigned int indice)
{
    mascara[indice >> 3] |= 1u << (indice & 0b111);
}

static bool bitMarcado(const unsigned char* mascara, unsigned int indice)
{
    return (mascara[indice >> 3] >> (indice & 0b111)) & 1;
}

// ================================================================ Combinaciones

unsigned int marcarCombinaciones(const unsigned char* tablero, unsigned int filas,
                                 unsigned int columnas, unsigned char* mascara)
{
    unsigned int combinaciones = 0;

    // Horizontal: recorre cada fila por tramos de fichas iguales
    for (unsigned int f = 0; f < filas; f++) {
        unsigned int inicio = 0;
        while (inicio < columnas) {
            unsigned char tipo = obtenerFichaFC(tablero, columnas, f, inicio);
            unsigned int fin = inicio + 1;
            while (fin < columnas && obtenerFichaFC(tablero, columnas, f, fin) == tipo) {
                fin++;
            }
            if (tipo < TIPOS_FICHA && fin - inicio >= 3) {
                for (unsigned int c = inicio; c < fin; c++) {
                    marcarBit(mascara, f * columnas + c);
                }
                combinaciones++;
            }
            inicio = fin;
        }
    }

    // Vertical: mismo recorrido por columnas
    for (unsigned int c = 0; c < columnas; c++) {
        unsigned int inicio = 0;
        while (inicio < filas) {
            unsigned char tipo = obtenerFichaFC(tablero, columnas, inicio, c);
            unsigned int fin = inicio + 1;
            while (fin < filas && obtenerFichaFC(tablero, columnas, fin, c) == tipo) {
                fin++;
            }
            if (tipo < TIPOS_FICHA && fin - inicio >= 3) {
                for (unsigned int f = inicio; f < fin; f++) {
                    marcarBit(mascara, f * columnas + c);
                }
                combinaciones++;
            }
            inicio = fin;
        }
    }
    return combinaciones;
}

unsigned int eliminarMarcadas(unsigned char* tablero, unsigned int filas,
                              unsigned int columnas, const unsigned char* mascara)
{
    unsigned int total = filas * columnas;
    unsigned int eliminadas = 0;
    for (unsigned int k = 0; k < total; k++) {
        if (bitMarcado(mascara, k)) {
            asignarFicha(tablero, k, FICHA_VACIA);
            eliminadas++;
        }
    }
    return eliminadas;
}

// ================================================================ Reorganizacion

// Por columna, de abajo hacia arriba: cada ficha no vacia baja a la
// siguiente posicion libre; lo que queda arriba se marca como vacio.
// Se recorre con k = filas..1 y f = k - 1 para usar solo unsigned
// (un unsigned nunca es < 0, asi que 'f >= 0' seria un ciclo infinito).
void aplicarGravedad(unsigned char* tablero, unsigned int filas, unsigned int columnas)
{
    for (unsigned int c = 0; c < columnas; c++) {
        unsigned int escritura = filas;   // la siguiente fila libre es escritura - 1
        for (unsigned int k = filas; k > 0; k--) {
            unsigned int f = k - 1;
            unsigned char ficha = obtenerFichaFC(tablero, columnas, f, c);
            if (ficha != FICHA_VACIA) {
                escritura--;
                if (f != escritura) {
                    asignarFichaFC(tablero, columnas, escritura, c, ficha);
                }
            }
        }
        for (unsigned int f = 0; f < escritura; f++) {
            asignarFichaFC(tablero, columnas, f, c, FICHA_VACIA);
        }
    }
}

void rellenarVacias(unsigned char* tablero, unsigned int filas, unsigned int columnas)
{
    unsigned int total = filas * columnas;
    for (unsigned int k = 0; k < total; k++) {
        if (obtenerFicha(tablero, k) == FICHA_VACIA) {
            asignarFicha(tablero, k, fichaAleatoria());
        }
    }
}

// Repite: detectar -> eliminar -> gravedad -> rellenar, mientras haya
// combinaciones. Cada vuelta con combinaciones cuenta como una cascada.
// Con estadisticas = nullptr no suma nada (se usa al estabilizar el inicio).
unsigned int procesarCascadas(unsigned char* tablero, unsigned int filas, unsigned int columnas,
                              unsigned int* estadisticas, bool mostrar)
{
    unsigned int bytesMascara = (filas * columnas + 7) >> 3;
    unsigned char* mascara = new unsigned char[bytesMascara]();
    unsigned int cascadas = 0;

    unsigned int combinaciones = marcarCombinaciones(tablero, filas, columnas, mascara);
    while (combinaciones > 0) {
        cascadas++;
        unsigned int eliminadas = eliminarMarcadas(tablero, filas, columnas, mascara);

        if (estadisticas != nullptr) {
            estadisticas[EST_COMBINACIONES]     += combinaciones;
            estadisticas[EST_FICHAS_ELIMINADAS] += eliminadas;
            estadisticas[EST_PUNTOS]            += eliminadas * PUNTOS_POR_FICHA * cascadas;
        }
        if (mostrar) {
            cout << "\n>> Cascada " << cascadas << ": " << combinaciones
                 << " combinacion(es), " << eliminadas << " fichas eliminadas\n";
            imprimirTablero(tablero, filas, columnas);
        }

        aplicarGravedad(tablero, filas, columnas);
        if (mostrar) {
            cout << "\nFichas desplazadas hacia abajo:\n";
            imprimirTablero(tablero, filas, columnas);
        }

        rellenarVacias(tablero, filas, columnas);
        if (mostrar) {
            cout << "\nNuevas fichas incorporadas:\n";
            imprimirTablero(tablero, filas, columnas);
        }

        for (unsigned int k = 0; k < bytesMascara; k++) {
            mascara[k] = 0;
        }
        combinaciones = marcarCombinaciones(tablero, filas, columnas, mascara);
    }

    delete[] mascara;
    if (estadisticas != nullptr) {
        estadisticas[EST_CASCADAS_ACTUAL] = cascadas;
        estadisticas[EST_CASCADAS_TOTAL] += cascadas;
    }
    return cascadas;
}

// ================================================================ Jugada del usuario

void eliminarFichaUsuario(unsigned char* tablero, unsigned int filas, unsigned int columnas,
                          unsigned int fila, unsigned int columna, unsigned int* estadisticas)
{
    asignarFichaFC(tablero, columnas, fila, columna, FICHA_VACIA);
    estadisticas[EST_ELIMINACIONES_USUARIO]++;
    estadisticas[EST_FICHAS_ELIMINADAS]++;

    cout << "\nFicha (" << fila << ", " << columna << ") eliminada:\n";
    imprimirTablero(tablero, filas, columnas);

    aplicarGravedad(tablero, filas, columnas);
    cout << "\nFichas desplazadas hacia abajo:\n";
    imprimirTablero(tablero, filas, columnas);

    rellenarVacias(tablero, filas, columnas);
    cout << "\nNuevas fichas incorporadas:\n";
    imprimirTablero(tablero, filas, columnas);

    procesarCascadas(tablero, filas, columnas, estadisticas, true);
}

// ================================================================ Modificacion estructural

// Deja en 0 todos los bits posteriores a las posiciones validas,
// para que el relleno quede siempre agrupado a la izquierda.
static void limpiarRelleno(unsigned char* tablero, unsigned int posiciones, unsigned int capacidad)
{
    unsigned int bit  = posiciones * 3;
    unsigned int byte = bit >> 3;
    unsigned int desp = bit & 0b111;
    if (byte < capacidad) {
        tablero[byte] &= (1u << desp) - 1;
        for (unsigned int k = byte + 1; k < capacidad; k++) {
            tablero[k] = 0;
        }
    }
}

// Regla del enunciado: al eliminar, solo se reasigna memoria si los bytes
// necesarios quedan por debajo del 65 % de los reservados.
// Se compara en enteros: necesarios * 100 < capacidad * 65
static bool debeReducir(unsigned int necesarios, unsigned int capacidad)
{
    return necesarios * 100 < capacidad * 65;
}

void agregarFila(unsigned char*& tablero, unsigned int& filas, unsigned int columnas,
                 unsigned int posicion, unsigned int& capacidad)
{
    unsigned char* nuevo = crearTablero(filas + 1, columnas);
    for (unsigned int f = 0; f < filas; f++) {
        unsigned int destino = (f < posicion) ? f : f + 1;
        for (unsigned int c = 0; c < columnas; c++) {
            asignarFichaFC(nuevo, columnas, destino, c, obtenerFichaFC(tablero, columnas, f, c));
        }
    }
    for (unsigned int c = 0; c < columnas; c++) {
        asignarFichaFC(nuevo, columnas, posicion, c, fichaAleatoria());
    }
    liberarTablero(tablero);
    tablero = nuevo;
    filas++;
    capacidad = bytesNecesarios(filas, columnas);
}

void eliminarFila(unsigned char*& tablero, unsigned int& filas, unsigned int columnas,
                  unsigned int posicion, unsigned int& capacidad)
{
    unsigned int nuevasFilas = filas - 1;
    unsigned int necesarios  = bytesNecesarios(nuevasFilas, columnas);

    if (debeReducir(necesarios, capacidad)) {
        unsigned char* nuevo = crearTablero(nuevasFilas, columnas);
        for (unsigned int f = 0; f < nuevasFilas; f++) {
            unsigned int origen = (f < posicion) ? f : f + 1;
            for (unsigned int c = 0; c < columnas; c++) {
                asignarFichaFC(nuevo, columnas, f, c, obtenerFichaFC(tablero, columnas, origen, c));
            }
        }
        liberarTablero(tablero);
        tablero = nuevo;
        capacidad = necesarios;
    } else {
        // En el mismo bloque: las fichas posteriores a la fila avanzan
        // 'columnas' posiciones hacia atras (destino < origen, recorrido hacia adelante).
        unsigned int total = nuevasFilas * columnas;
        for (unsigned int k = posicion * columnas; k < total; k++) {
            asignarFicha(tablero, k, obtenerFicha(tablero, k + columnas));
        }
        limpiarRelleno(tablero, total, capacidad);
    }
    filas = nuevasFilas;
}

void agregarColumna(unsigned char*& tablero, unsigned int filas, unsigned int& columnas,
                    unsigned int posicion, unsigned int& capacidad)
{
    unsigned int nuevasColumnas = columnas + 1;
    unsigned char* nuevo = crearTablero(filas, nuevasColumnas);
    for (unsigned int f = 0; f < filas; f++) {
        for (unsigned int c = 0; c < nuevasColumnas; c++) {
            unsigned char ficha;
            if (c == posicion) {
                ficha = fichaAleatoria();
            } else {
                unsigned int origen = (c < posicion) ? c : c - 1;
                ficha = obtenerFichaFC(tablero, columnas, f, origen);
            }
            asignarFichaFC(nuevo, nuevasColumnas, f, c, ficha);
        }
    }
    liberarTablero(tablero);
    tablero = nuevo;
    columnas = nuevasColumnas;
    capacidad = bytesNecesarios(filas, columnas);
}

void eliminarColumna(unsigned char*& tablero, unsigned int filas, unsigned int& columnas,
                     unsigned int posicion, unsigned int& capacidad)
{
    unsigned int nuevasColumnas = columnas - 1;
    unsigned int necesarios     = bytesNecesarios(filas, nuevasColumnas);

    if (debeReducir(necesarios, capacidad)) {
        unsigned char* nuevo = crearTablero(filas, nuevasColumnas);
        for (unsigned int f = 0; f < filas; f++) {
            for (unsigned int c = 0; c < nuevasColumnas; c++) {
                unsigned int origen = (c < posicion) ? c : c + 1;
                asignarFichaFC(nuevo, nuevasColumnas, f, c, obtenerFichaFC(tablero, columnas, f, origen));
            }
        }
        liberarTablero(tablero);
        tablero = nuevo;
        capacidad = necesarios;
    } else {
        // En el mismo bloque: el indice destino nunca supera al de origen,
        // asi que recorrer hacia adelante no pisa fichas sin leer.
        for (unsigned int f = 0; f < filas; f++) {
            for (unsigned int c = 0; c < nuevasColumnas; c++) {
                unsigned int origen = (c < posicion) ? c : c + 1;
                asignarFicha(tablero, f * nuevasColumnas + c,
                             obtenerFicha(tablero, f * columnas + origen));
            }
        }
        limpiarRelleno(tablero, filas * nuevasColumnas, capacidad);
    }
    columnas = nuevasColumnas;
}