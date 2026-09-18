#include "tablerobits.h"
#include <cstdlib>
#include <iostream>
using namespace std;

// Memoria

// 3*F*C bits redondeados hacia arriba al byte: (bits + 7) / 8
unsigned int bytesNecesarios(unsigned int filas, unsigned int columnas)
{
    return (filas * columnas * 3 + 7) >> 3;
}

// Bloque contiguo de bytes, inicializado en cero (los bits sobrantes del
// ultimo byte quedan en 0 y a la izquierda, como exige el enunciado).
unsigned char* crearTablero(unsigned int filas, unsigned int columnas)
{
    return new unsigned char[bytesNecesarios(filas, columnas)]();
}

void liberarTablero(unsigned char*& tablero)
{
    delete[] tablero;
    tablero = nullptr;
}

//  Acceso

// indice -> bit inicial = indice*3 -> byte = bit/8, desplazamiento = bit%8
// Si desplazamiento <= 5 la ficha cabe en un byte; si es 6 o 7 se parte
// entre el byte actual (bits altos) y el siguiente (bits bajos).
unsigned char obtenerFicha(const unsigned char* tablero, unsigned int indice)
{
    unsigned int bit  = indice * 3;
    unsigned int byte = bit >> 3;
    unsigned int desp = bit & 0b111;

    unsigned int valor = tablero[byte] >> desp;
    if (desp > 5) {
        unsigned int siguiente = tablero[byte + 1];
        valor |= siguiente << (8 - desp);
    }
    return valor & 0b111;
}

void asignarFicha(unsigned char* tablero, unsigned int indice, unsigned char valor)
{
    valor &= 0b111;
    unsigned int bit  = indice * 3;
    unsigned int byte = bit >> 3;
    unsigned int desp = bit & 0b111;

    // Parte que cae en el byte actual (se trunca a 8 bits al asignar)
    unsigned int codigo = valor;
    tablero[byte] = (tablero[byte] & ~(0b111u << desp)) | (codigo << desp);

    // Parte que desborda al siguiente byte (1 o 2 bits)
    if (desp > 5) {
        unsigned int  resto   = desp - 5;
        unsigned char mascara = (1u << resto) - 1;
        tablero[byte + 1] = (tablero[byte + 1] & ~mascara) | (valor >> (8 - desp));
    }
}

unsigned char obtenerFichaFC(const unsigned char* tablero, unsigned int columnas,
                             unsigned int fila, unsigned int columna)
{
    return obtenerFicha(tablero, fila * columnas + columna);
}

void asignarFichaFC(unsigned char* tablero, unsigned int columnas,
                    unsigned int fila, unsigned int columna, unsigned char valor)
{
    asignarFicha(tablero, fila * columnas + columna, valor);
}

//  Generacion

// Se toman 3 bits aleatorios (0..7) y se descartan 6 y 7:
// cada uno de los 6 tipos queda con probabilidad exactamente 1/6.
unsigned char fichaAleatoria()
{
    unsigned char ficha;
    do {
        ficha = rand() & 0b111;
    } while (ficha >= TIPOS_FICHA);
    return ficha;
}

void llenarAleatorio(unsigned char* tablero, unsigned int filas, unsigned int columnas)
{
    unsigned int total = filas * columnas;
    for (unsigned int k = 0; k < total; k++) {
        asignarFicha(tablero, k, fichaAleatoria());
    }
}

//  Visualizacion

static const char SIMBOLOS[8] = { '#', '$', '%', '&', '*', '+', '.', '@' };

void imprimirTablero(const unsigned char* tablero, unsigned int filas, unsigned int columnas)
{
    cout << "    ";
    for (unsigned int c = 0; c < columnas; c++) {
        cout << (c < 10 ? " " : "") << c << ' ';
    }
    cout << '\n';
    for (unsigned int f = 0; f < filas; f++) {
        cout << (f < 10 ? " " : "") << f << " |";
        for (unsigned int c = 0; c < columnas; c++) {
            cout << ' ' << SIMBOLOS[obtenerFichaFC(tablero, columnas, f, c)] << ' ';
        }
        cout << '\n';
    }
}

// Cada posicion como su codigo de 3 bits (bit 2 a bit 0)
void imprimirBinario(const unsigned char* tablero, unsigned int filas, unsigned int columnas)
{
    for (unsigned int f = 0; f < filas; f++) {
        for (unsigned int c = 0; c < columnas; c++) {
            unsigned char v = obtenerFichaFC(tablero, columnas, f, c);
            for (int b = 2; b >= 0; b--) {
                cout << ((v >> b) & 1);
            }
            cout << ' ';
        }
        cout << '\n';
    }
}

// Memoria cruda: cada byte de bit 7 a bit 0
void imprimirBytes(const unsigned char* tablero, unsigned int filas, unsigned int columnas)
{
    unsigned int n = bytesNecesarios(filas, columnas);
    for (unsigned int k = 0; k < n; k++) {
        cout << "Byte " << k << ": ";
        for (int b = 7; b >= 0; b--) {
            cout << ((tablero[k] >> b) & 1);
        }
        cout << '\n';
    }
}