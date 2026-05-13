#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "map.h"

struct Jugador {
    float x;
    float y;
    int   fila;
    int   columna;
    float velocidad;
    bool  moviendose;
    int   animFrame;
    float animTimer;
};

void inicializarJugador(Jugador &p, int fila, int columna);
void manejarEntrada(Jugador &p, float dt);
void dibujarJugador(const Jugador &p, sf::RenderWindow &ventana);

#endif
