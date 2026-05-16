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
    int   habitacion;
};

void inicializarEnemigos(Enemigo enemigos[], int &numEnemigos);
void actualizarEnemigos(Enemigo enemigos[], int numEnemigos,
                        const Habitacion &habitacion, int habitacionActual);
void dibujarEnemigos(const Enemigo enemigos[], int numEnemigos,
                     sf::RenderWindow &ventana, int habitacionActual);

#endif
