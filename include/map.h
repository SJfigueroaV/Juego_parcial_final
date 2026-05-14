#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>

const int MAX_FILAS        = 20;
const int MAX_COLUMNAS     = 26;
const int TAM_TILE         = 32;
const int MAX_HABITACIONES = 6;

const int T_PARED  = 0;
const int T_SUELO  = 1;
const int T_PUERTA = 2;

struct Puerta {
    int  aHabitacion;
    int  fila, columna;
    bool bloqueada;
};

struct Habitacion {
    int    cuadricula[MAX_COLUMNAS][MAX_FILAS];
    Puerta puertas[8];
    int    numPuertas;
};

void inicializarHabitaciones(Habitacion habitaciones[], int &numHabitaciones);
void dibujarHabitacion(const Habitacion &habitacion, sf::RenderWindow &ventana);
bool esPared(const Habitacion &habitacion, int fila, int columna);
const Puerta *obtenerPuertaEn(const Habitacion &habitacion, int fila, int columna);

#endif
