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

void verificarEfectosObjetos(Objeto slots[], int numSlots,
                             Jugador &jugador, Enemigo enemigos[], int numEnemigos,
                             int habitacionActual) {
    (void)jugador;
    for (int s = 0; s < numSlots; s++) {
        Objeto &obj = slots[s];
        if (obj.tipo == NINGUNO || !obj.enSuelo) continue;
        if (obj.habitacion != habitacionActual) continue;

        if (obj.tipo == TRAP) {
            if (obj.active) continue;
            for (int e = 0; e < numEnemigos; e++) {
                if (!enemigos[e].vivo) continue;
                if (enemigos[e].habitacion != habitacionActual) continue;
                if (enemigos[e].fila == obj.fila && enemigos[e].columna == obj.columna) {
                    enemigos[e].intervaloMov = 999999; // congelado efectivo
                    obj.active = true;
                    break;
                }
            }
        } else if (obj.tipo == BARREL) {
            if (obj.active) continue;
            for (int e = 0; e < numEnemigos; e++) {
                if (!enemigos[e].vivo) continue;
                if (enemigos[e].habitacion != habitacionActual) continue;
                if (enemigos[e].fila == obj.fila && enemigos[e].columna == obj.columna) {
                    enemigos[e].dirFila    = -enemigos[e].dirFila;
                    enemigos[e].dirColumna = -enemigos[e].dirColumna;
                    obj.active = true;
                    obj.effectTimer = 0.8f;
                    break;
                }
            }
        }
    }
}

void actualizarTimersObjetos(Objeto slots[], int numSlots,
                             Jugador &jugador, float dt) {
    for (int s = 0; s < numSlots; s++) {
        Objeto &obj = slots[s];
        if (obj.tipo == NINGUNO) continue;

        if (obj.tipo == SPEEDBOOST) {
            if (obj.active) {
                obj.effectTimer -= dt;
                if (obj.effectTimer <= 0.0f) {
                    obj.effectTimer    = 0.0f;
                    obj.active         = false;
                    jugador.velocidad  = 210.0f;
                    obj.cooldownTimer  = 5.0f;
                }
            } else if (obj.cooldownTimer > 0.0f) {
                obj.cooldownTimer -= dt;
                if (obj.cooldownTimer < 0.0f) obj.cooldownTimer = 0.0f;
            }
        } else if (obj.tipo == BARREL && obj.active) {
            obj.effectTimer -= dt;
            if (obj.effectTimer <= 0.0f) {
                obj.effectTimer = 0.0f;
                obj.active      = false;
            }
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

void dibujarObjetosSuelo(const Objeto slots[], int numSlots,
                         sf::RenderWindow &ventana, int habitacionActual) {
    sf::RectangleShape rect(sf::Vector2f{(float)TAM_TILE * 0.6f, (float)TAM_TILE * 0.6f});
    for (int s = 0; s < numSlots; s++) {
        const Objeto &obj = slots[s];
        if (!obj.enSuelo || obj.tipo == NINGUNO) continue;
        if (obj.habitacion != habitacionActual) continue;

        if (obj.tipo == TRAP)        rect.setFillColor(sf::Color(120, 60, 200));
        else if (obj.tipo == BARREL) rect.setFillColor(sf::Color(180, 100, 30));
        else                          rect.setFillColor(sf::Color(80, 200, 80));

        rect.setPosition(sf::Vector2f{
            (float)(obj.columna * TAM_TILE) + TAM_TILE * 0.2f,
            (float)(obj.fila    * TAM_TILE) + TAM_TILE * 0.2f
        });
        ventana.draw(rect);
    }
}
