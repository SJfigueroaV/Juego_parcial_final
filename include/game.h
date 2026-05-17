#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"

enum EstadoJuego {
    JUGANDO,
    VICTORIA,
    DERROTA
};

bool verificarVictoria(int habitacionActual);
bool verificarDerrota(const Jugador &p, const Enemigo enemigos[], int numEnemigos,
                     int habitacionActual);

void ejecutarJuego();

#endif
