#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include "map.h"
#include "player.h"

enum TipoEnemigo {
    PERSEGUIDOR,
    PATRULLERO
};

const int MAX_ENEMIES = 10;

struct Enemigo {
    TipoEnemigo tipo;
    int   fila;
    int   columna;
    float x;
    float y;
    bool  vivo;
    int   dirFila;
    int   dirColumna;
    int   intervaloMov;
    int   tiempoMov;
    float frozenTimer;
    float slowTimer;
    float boostTimer;
    float knockbackVX;
    float knockbackVY;
    int   habitacion;
    sf::RectangleShape shape;
};

void inicializarEnemigos(Enemigo enemies[], int &count);
void actualizarEnemigos(Enemigo enemies[], int count, const Jugador &player,
                   const Habitacion &room, float dt, int currentRoom,
                   const int barrelRows[], const int barrelCols[], int barrelCount);
void dibujarEnemigos(const Enemigo enemies[], int count,
                 sf::RenderWindow &window, int currentRoom);
void aumentarDificultad(Enemigo enemies[], int count, int level);

#endif
