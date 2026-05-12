#include "../include/map.h"

void inicializarHabitacionVacia(Habitacion &habitacion) {
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        for (int f = 0; f < MAX_FILAS; f++) {
            habitacion.cuadricula[c][f] = T_SUELO;
        }
    }

    // Paredes en los bordes
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        habitacion.cuadricula[c][0]              = T_PARED;
        habitacion.cuadricula[c][MAX_FILAS - 1]  = T_PARED;
    }
    for (int f = 0; f < MAX_FILAS; f++) {
        habitacion.cuadricula[0][f]                 = T_PARED;
        habitacion.cuadricula[MAX_COLUMNAS - 1][f]  = T_PARED;
    }
}
