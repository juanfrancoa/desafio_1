#ifndef TABLEROBITS_H
#define TABLEROBITS_H

// Codificacion (3 bits por posicion):
// 000..101 -> seis tipos de ficha, 110 -> posicion vacia, 111 -> estado especial
const unsigned char TIPOS_FICHA    = 6;
const unsigned char FICHA_VACIA    = 0b110;
const unsigned char FICHA_ESPECIAL = 0b111;

// Memoria
unsigned int   bytesNecesarios(unsigned int filas, unsigned int columnas);
unsigned char* crearTablero(unsigned int filas, unsigned int columnas);
void           liberarTablero(unsigned char*& tablero);

// Acceso a una ficha por indice logico (fila * columnas + columna)
unsigned char obtenerFicha(const unsigned char* tablero, unsigned int indice);
void          asignarFicha(unsigned char* tablero, unsigned int indice, unsigned char valor);

// Acceso por (fila, columna)
unsigned char obtenerFichaFC(const unsigned char* tablero, unsigned int columnas,
                             unsigned int fila, unsigned int columna);
void          asignarFichaFC(unsigned char* tablero, unsigned int columnas,
                    unsigned int fila, unsigned int columna, unsigned char valor);

// Generacion y visualizacion
unsigned char fichaAleatoria();
void llenarAleatorio(unsigned char* tablero, unsigned int filas, unsigned int columnas);
void imprimirTablero(const unsigned char* tablero, unsigned int filas, unsigned int columnas);
void imprimirBinario(const unsigned char* tablero, unsigned int filas, unsigned int columnas);
void imprimirBytes(const unsigned char* tablero, unsigned int filas, unsigned int columnas);

#endif