#include "../include/items.h"
#include "../include/map.h"

void verificarRecoleccion(Inventario &inv, Objeto objetosMundo[], int numObjetos,
                          const Jugador &p) {
    for (int i = 0; i < numObjetos; i++) {
        if (objetosMundo[i].tipo == NINGUNO) continue;
        if (objetosMundo[i].fila == p.fila && objetosMundo[i].columna == p.columna) {
            if (agregarObjeto(inv, objetosMundo[i].tipo)) {
                objetosMundo[i].tipo = NINGUNO;
            }
            return;
        }
    }
}

void dibujarObjetosMundo(const Objeto objetosMundo[], int numObjetos,
                         sf::RenderWindow &ventana, int habitacionActual) {
    sf::RectangleShape rect(sf::Vector2f{(float)TAM_TILE * 0.5f, (float)TAM_TILE * 0.5f});
    rect.setFillColor(sf::Color(255, 215, 0));
    for (int i = 0; i < numObjetos; i++) {
        if (objetosMundo[i].tipo == NINGUNO) continue;
        if (objetosMundo[i].habitacion != habitacionActual) continue;
        rect.setPosition(sf::Vector2f{
            (float)(objetosMundo[i].columna * TAM_TILE) + TAM_TILE * 0.25f,
            (float)(objetosMundo[i].fila    * TAM_TILE) + TAM_TILE * 0.25f
        });
        ventana.draw(rect);
    }
}
