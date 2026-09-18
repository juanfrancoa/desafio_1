#ifndef LOGICAJUEGO_H
#define LOGICAJUEGO_H

// Indices del arreglo de estadisticas (no se permite struct)
const unsigned char EST_ELIMINACIONES_USUARIO = 0;
const unsigned char EST_FICHAS_ELIMINADAS     = 1;
const unsigned char EST_COMBINACIONES         = 2;
const unsigned char EST_CASCADAS_ACTUAL       = 3;
const unsigned char EST_CASCADAS_TOTAL        = 4;
const unsigned char EST_PUNTOS                = 5;
const unsigned char NUM_ESTADISTICAS          = 6;

// Puntuacion: cada ficha eliminada por combinacion vale
// PUNTOS_POR_FICHA x numero de cascada (1ra cascada x1, 2da x2, ...)
const unsigned int PUNTOS_POR_FICHA = 10;

// Combinaciones
unsigned int marcarCombinaciones(const unsigned char* tablero, unsigned int filas,
                                 unsigned int columnas, unsigned char* mascara);
unsigned int eliminarMarcadas(unsigned char* tablero, unsigned int filas,
                              unsigned int columnas, const unsigned char* mascara);

// Reorganizacion
void aplicarGravedad(unsigned char* tablero, unsigned int filas, unsigned int columnas);
void rellenarVacias(unsigned char* tablero, unsigned int filas, unsigned int columnas);
unsigned int procesarCascadas(unsigned char* tablero, unsigned int filas, unsigned int columnas,
                              unsigned int* estadisticas, bool mostrar);

// Jugada del usuario
void eliminarFichaUsuario(unsigned char* tablero, unsigned int filas, unsigned int columnas,
                          unsigned int fila, unsigned int columna, unsigned int* estadisticas);

// Modificacion estructural
void agregarFila(unsigned char*& tablero, unsigned int& filas, unsigned int columnas,
                 unsigned int posicion, unsigned int& capacidad);
void eliminarFila(unsigned char*& tablero, unsigned int& filas, unsigned int columnas,
                  unsigned int posicion, unsigned int& capacidad);
void agregarColumna(unsigned char*& tablero, unsigned int filas, unsigned int& columnas,
                    unsigned int posicion, unsigned int& capacidad);
void eliminarColumna(unsigned char*& tablero, unsigned int filas, unsigned int& columnas,
                     unsigned int posicion, unsigned int& capacidad);

#endif