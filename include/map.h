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

struct Habitacion {
    int cuadricula[MAX_COLUMNAS][MAX_FILAS];
};

void inicializarHabitacionVacia(Habitacion &habitacion);

#endif
