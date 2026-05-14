#include "../include/map.h"

static void llenarBordes(Habitacion &h) {
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        for (int f = 0; f < MAX_FILAS; f++) {
            h.cuadricula[c][f] = T_SUELO;
        }
    }
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        h.cuadricula[c][0]              = T_PARED;
        h.cuadricula[c][MAX_FILAS - 1]  = T_PARED;
    }
    for (int f = 0; f < MAX_FILAS; f++) {
        h.cuadricula[0][f]                 = T_PARED;
        h.cuadricula[MAX_COLUMNAS - 1][f]  = T_PARED;
    }
    h.numPuertas = 0;
}

static void agregarPuerta(Habitacion &h, int fila, int columna, int aHabitacion, bool bloqueada) {
    h.cuadricula[columna][fila]     = T_PUERTA;
    h.cuadricula[columna + 1][fila] = T_PUERTA;
    h.puertas[h.numPuertas].fila        = fila;
    h.puertas[h.numPuertas].columna     = columna;
    h.puertas[h.numPuertas].aHabitacion = aHabitacion;
    h.puertas[h.numPuertas].bloqueada   = bloqueada;
    h.numPuertas++;
    h.puertas[h.numPuertas].fila        = fila;
    h.puertas[h.numPuertas].columna     = columna + 1;
    h.puertas[h.numPuertas].aHabitacion = aHabitacion;
    h.puertas[h.numPuertas].bloqueada   = bloqueada;
    h.numPuertas++;
}

void inicializarHabitaciones(Habitacion habitaciones[], int &numHabitaciones) {
    numHabitaciones = MAX_HABITACIONES;

    // Habitacion 0 → este → 1
    llenarBordes(habitaciones[0]);
    agregarPuerta(habitaciones[0], MAX_FILAS / 2, MAX_COLUMNAS - 1, 1, false);

    // Habitacion 1 → oeste → 0; sur → 2
    llenarBordes(habitaciones[1]);
    agregarPuerta(habitaciones[1], MAX_FILAS / 2, 0, 0, false);
    agregarPuerta(habitaciones[1], MAX_FILAS - 1, MAX_COLUMNAS / 2 - 1, 2, false);

    // Habitacion 2 → norte → 1; este → 3 (BLOQUEADA); sur → 4
    llenarBordes(habitaciones[2]);
    agregarPuerta(habitaciones[2], 0, MAX_COLUMNAS / 2 - 1, 1, false);
    agregarPuerta(habitaciones[2], MAX_FILAS / 2, MAX_COLUMNAS - 1, 3, true);
    agregarPuerta(habitaciones[2], MAX_FILAS - 1, MAX_COLUMNAS / 2 - 1, 4, false);

    // Habitacion 3 → oeste → 2 (sala final, sin más puertas)
    llenarBordes(habitaciones[3]);
    agregarPuerta(habitaciones[3], MAX_FILAS / 2, 0, 2, false);

    // Habitacion 4 → norte → 2; sur → 5
    llenarBordes(habitaciones[4]);
    agregarPuerta(habitaciones[4], 0, MAX_COLUMNAS / 2 - 1, 2, false);
    agregarPuerta(habitaciones[4], MAX_FILAS - 1, MAX_COLUMNAS / 2 - 1, 5, false);

    // Habitacion 5 → norte → 4
    llenarBordes(habitaciones[5]);
    agregarPuerta(habitaciones[5], 0, MAX_COLUMNAS / 2 - 1, 4, false);
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

const Puerta *obtenerPuertaEn(const Habitacion &habitacion, int fila, int columna) {
    for (int i = 0; i < habitacion.numPuertas; i++) {
        if (habitacion.puertas[i].fila == fila && habitacion.puertas[i].columna == columna) {
            return &habitacion.puertas[i];
        }
    }
    return nullptr;
}
