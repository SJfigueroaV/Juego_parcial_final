#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "player.h"
#include "enemy.h"
#include "map.h"
#include "inventory.h"

const int DUNGEON_W  = 832;
const int INV_PANEL_W = 100;
const int WINDOW_W   = DUNGEON_W + INV_PANEL_W;
const int WINDOW_H   = 640;

enum EstadoJuego {
    JUGANDO,
    VICTORIA,
    DERROTA
};

const int MAX_CHESTS = 16;

struct Cofre {
    int      fila, columna, habitacion;
    bool     opened;
    TipoObjeto itemType;
};

bool verificarVictoria(int currentRoom);
bool verificarDerrota(const Jugador &p, const Enemigo enemies[], int count, int currentRoom);
void ejecutarJuego();

#endif
