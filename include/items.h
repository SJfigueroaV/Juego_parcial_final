#ifndef ITEMS_H
#define ITEMS_H

#include "inventory.h"
#include "player.h"
#include "enemy.h"
#include <SFML/Graphics.hpp>

void verificarRecoleccion(Inventario &inv, Objeto objetosMundo[], int numObjetos,
                          const Jugador &p);

void verificarEfectosObjetos(Objeto slots[], int numSlots,
                             Jugador &jugador, Enemigo enemigos[], int numEnemigos,
                             int habitacionActual);

void actualizarTimersObjetos(Objeto slots[], int numSlots,
                             Jugador &jugador, float dt);

void dibujarObjetosMundo(const Objeto objetosMundo[], int numObjetos,
                         sf::RenderWindow &ventana, int habitacionActual);

void dibujarObjetosSuelo(const Objeto slots[], int numSlots,
                         sf::RenderWindow &ventana, int habitacionActual);

#endif
