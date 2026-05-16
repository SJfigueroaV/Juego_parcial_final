#include "../include/enemy.h"
#include <cstdlib>

static int signo(int x) {
    if (x > 0) return  1;
    if (x < 0) return -1;
    return 0;
}

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
    inicializarEnemigo(enemigos[numEnemigos++], PATRULLERO, 0,  1, 12,  1, 0, 4);
    inicializarEnemigo(enemigos[numEnemigos++], PATRULLERO, 0, 18, 13, -1, 0, 4);
    inicializarEnemigo(enemigos[numEnemigos++], PERSEGUIDOR, 4, 17,  3, 0,  1, 9);
    inicializarEnemigo(enemigos[numEnemigos++], PERSEGUIDOR, 4, 17, 13, 0,  1, 9);
    inicializarEnemigo(enemigos[numEnemigos++], PERSEGUIDOR, 4, 17, 22, 0, -1, 9);
}

static void actualizarPerseguidor(Enemigo &e, const Jugador &jugador,
                                   const Habitacion &habitacion) {
    int dFila    = jugador.fila    - e.fila;
    int dColumna = jugador.columna - e.columna;

    if (dFila == 0 && dColumna == 0) return;

    int sr = signo(dFila);
    int sc = signo(dColumna);

    bool diagOk = !esPared(habitacion, e.fila + sr, e.columna + sc);
    bool filaOk = !esPared(habitacion, e.fila + sr, e.columna);
    bool colOk  = !esPared(habitacion, e.fila, e.columna + sc);

    if (sr != 0 && sc != 0 && diagOk) {
        e.fila    += sr;
        e.columna += sc;
    } else if (filaOk) {
        if (std::abs(dFila) >= std::abs(dColumna)) {
            e.fila += sr;
        } else if (colOk) {
            e.columna += sc;
        } else {
            e.fila += sr;
        }
    } else if (colOk) {
        e.columna += sc;
    }
}

static void actualizarPatrullero(Enemigo &e, const Habitacion &habitacion) {
    int siguienteFila    = e.fila + e.dirFila;
    int siguienteColumna = e.columna + e.dirColumna;

    if (esPared(habitacion, siguienteFila, siguienteColumna)) {
        e.dirFila    = -e.dirFila;
        e.dirColumna = -e.dirColumna;
        siguienteFila    = e.fila + e.dirFila;
        siguienteColumna = e.columna + e.dirColumna;
    }

    if (!esPared(habitacion, siguienteFila, siguienteColumna)) {
        e.fila    = siguienteFila;
        e.columna = siguienteColumna;
    }
}

void actualizarEnemigos(Enemigo enemigos[], int numEnemigos,
                        const Jugador &jugador,
                        const Habitacion &habitacion, int habitacionActual) {
    for (int i = 0; i < numEnemigos; i++) {
        if (!enemigos[i].vivo || enemigos[i].habitacion != habitacionActual) continue;

        enemigos[i].tiempoMov--;
        if (enemigos[i].tiempoMov > 0) continue;
        enemigos[i].tiempoMov = enemigos[i].intervaloMov;

        if (enemigos[i].tipo == PATRULLERO) {
            actualizarPatrullero(enemigos[i], habitacion);
        } else if (enemigos[i].tipo == PERSEGUIDOR) {
            actualizarPerseguidor(enemigos[i], jugador, habitacion);
        }

        enemigos[i].x = (float)(enemigos[i].columna * TAM_TILE);
        enemigos[i].y = (float)(enemigos[i].fila    * TAM_TILE);
    }
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
