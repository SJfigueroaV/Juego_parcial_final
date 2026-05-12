#include "../include/map.h"

void inicializarHabitacionVacia(Habitacion &habitacion) {
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        for (int f = 0; f < MAX_FILAS; f++) {
            habitacion.cuadricula[c][f] = T_SUELO;
        }
    }

    for (int c = 0; c < MAX_COLUMNAS; c++) {
        habitacion.cuadricula[c][0]              = T_PARED;
        habitacion.cuadricula[c][MAX_FILAS - 1]  = T_PARED;
    }
    for (int f = 0; f < MAX_FILAS; f++) {
        habitacion.cuadricula[0][f]                 = T_PARED;
        habitacion.cuadricula[MAX_COLUMNAS - 1][f]  = T_PARED;
    }
}

void dibujarHabitacion(const Habitacion &habitacion, sf::RenderWindow &ventana) {
    sf::RectangleShape rect(sf::Vector2f{(float)TAM_TILE, (float)TAM_TILE});
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        for (int f = 0; f < MAX_FILAS; f++) {
            int id = habitacion.cuadricula[c][f];
            sf::Color color;
            if (id == T_PARED) {
                color = sf::Color(80, 80, 80);
            } else if (id == T_SUELO) {
                color = sf::Color(0x25, 0x13, 0x1A);
            } else {
                color = sf::Color(139, 90, 43);
            }
            rect.setFillColor(color);
            rect.setPosition(sf::Vector2f{(float)(c * TAM_TILE), (float)(f * TAM_TILE)});
            ventana.draw(rect);
        }
    }
}

bool esPared(const Habitacion &habitacion, int fila, int columna) {
    if (fila < 0 || fila >= MAX_FILAS || columna < 0 || columna >= MAX_COLUMNAS) {
        return true;
    }
    return habitacion.cuadricula[columna][fila] == T_PARED;
}
