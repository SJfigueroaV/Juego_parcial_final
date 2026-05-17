#ifndef ITEMS_H
#define ITEMS_H

#include "inventory.h"
#include "player.h"
#include <SFML/Graphics.hpp>

void verificarRecoleccion(Inventario &inv, Objeto objetosMundo[], int numObjetos,
                          const Jugador &p);

void dibujarObjetosMundo(const Objeto objetosMundo[], int numObjetos,
                         sf::RenderWindow &ventana, int habitacionActual);

#endif
