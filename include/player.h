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

void inicializarJugador(Jugador &p, int startRow, int startCol);
void manejarEntrada(Jugador &p, const Habitacion &room, float dt);
void dibujarJugador(const Jugador &p, sf::RenderWindow &window);

void cargarTexturaJugadorReposo(const std::string &path);
void cargarTexturaJugadorCaminando(const std::string &path);

#endif
