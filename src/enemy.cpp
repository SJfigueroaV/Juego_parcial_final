#include "../include/enemy.h"

static void inicializarEnemigo(Enemigo &e, TipoEnemigo tipo, int habitacion,
                                int fila, int columna,
                                int dirFila, int dirColumna,
                                int intervaloMov) {
    e.tipo          = tipo;
    e.habitacion    = habitacion;
    e.fila          = fila;
    e.columna       = columna;
    e.x             = (float)(columna * TAM_TILE);
    e.y             = (float)(fila * TAM_TILE);
    e.vivo          = true;
    e.dirFila       = dirFila;
    e.dirColumna    = dirColumna;
    e.intervaloMov  = intervaloMov;
    e.tiempoMov     = intervaloMov;
}

void inicializarEnemigos(Enemigo enemigos[], int &numEnemigos) {
    numEnemigos = 0;
    // Habitacion 0: enemigos patrulla
    inicializarEnemigo(enemigos[numEnemigos++], PATRULLERO, 0,  1, 12,  1, 0, 4);
    inicializarEnemigo(enemigos[numEnemigos++], PATRULLERO, 0, 18, 13, -1, 0, 4);
    // Habitacion 4: chasers
    inicializarEnemigo(enemigos[numEnemigos++], PERSEGUIDOR, 4, 17,  3, 0,  1, 9);
    inicializarEnemigo(enemigos[numEnemigos++], PERSEGUIDOR, 4, 17, 13, 0,  1, 9);
    inicializarEnemigo(enemigos[numEnemigos++], PERSEGUIDOR, 4, 17, 22, 0, -1, 9);
}

void dibujarEnemigos(const Enemigo enemigos[], int numEnemigos,
                     sf::RenderWindow &ventana, int habitacionActual) {
    sf::RectangleShape rect(sf::Vector2f{(float)TAM_TILE * 0.65f, (float)TAM_TILE * 0.65f});
    for (int i = 0; i < numEnemigos; i++) {
        if (!enemigos[i].vivo || enemigos[i].habitacion != habitacionActual) continue;
        if (enemigos[i].tipo == PERSEGUIDOR) {
            rect.setFillColor(sf::Color(200, 50, 50));
        } else {
            rect.setFillColor(sf::Color(230, 140, 30));
        }
        rect.setPosition(sf::Vector2f{
            enemigos[i].x + (float)TAM_TILE * 0.175f,
            enemigos[i].y + (float)TAM_TILE * 0.175f
        });
        ventana.draw(rect);
    }
}
