#ifndef ITEMS_H
#define ITEMS_H

#include "inventory.h"
#include "player.h"
#include "enemy.h"
#include <SFML/Graphics.hpp>

void verificarRecoleccion(Inventario &inv, Objeto worldItems[], int itemCount,
                     const Jugador &p);

void verificarEfectosObjetos(Objeto inventorySlots[], int slotCount,
                      Jugador &player, Enemigo enemies[], int enemyCount,
                      int currentRoom);

void actualizarTimersObjetos(Objeto inventorySlots[], int slotCount,
                      Jugador &player, Enemigo enemies[], int enemyCount,
                      float dt);

void dibujarObjetosSuelo(const Objeto inventorySlots[], int slotCount,
                    sf::RenderWindow &window, int currentRoom);

#endif
