#include "../include/items.h"
#include "../include/map.h"
#include <cmath>
#include <cstdio>

static const sf::IntRect TRAP_RECT  (sf::Vector2i{  0, 64}, sf::Vector2i{16, 16});
static const sf::IntRect BARREL_RECT(sf::Vector2i{ 64, 64}, sf::Vector2i{16, 16});
static const sf::IntRect SPEED_RECT (sf::Vector2i{160, 16}, sf::Vector2i{16, 16});

static void dibujarSpriteProp(sf::RenderWindow &window,
                           const sf::IntRect &rect,
                           float worldX, float worldY) {
    const sf::Texture *tex = obtenerTexturaPuerta();
    if (!tex) {
        sf::RectangleShape fb(sf::Vector2f{(float)TAM_TILE, (float)TAM_TILE});
        fb.setPosition(sf::Vector2f{worldX, worldY});
        fb.setFillColor(sf::Color(180, 90, 20));
        window.draw(fb);
        return;
    }
    sf::Sprite spr(*tex);
    spr.setTextureRect(rect);
    float sc = (float)TAM_TILE / 16.0f;
    spr.setScale(sf::Vector2f{sc, sc});
    spr.setPosition(sf::Vector2f{worldX, worldY});
    window.draw(spr);
}

void verificarRecoleccion(Inventario &inv, Objeto worldItems[], int itemCount,
                     const Jugador &p) {
    for (int i = 0; i < itemCount; i++) {
        if (worldItems[i].tipo == NINGUNO) continue;
        if (worldItems[i].fila == p.fila && worldItems[i].columna == p.columna) {
            if (agregarObjeto(inv, worldItems[i].tipo)) {
                worldItems[i].tipo = NINGUNO;
            }
            return;
        }
    }
}

void verificarEfectosObjetos(Objeto inventorySlots[], int slotCount,
                      Jugador &player, Enemigo enemies[], int enemyCount,
                      int currentRoom) {
    (void)player;
    for (int s = 0; s < slotCount; s++) {
        Objeto &item = inventorySlots[s];

        if (item.tipo == NINGUNO || !item.enSuelo) continue;
        if (item.habitacion != currentRoom)         continue;

        if (item.tipo == TRAP) {
            if (item.active) continue;
            for (int e = 0; e < enemyCount; e++) {
                if (!enemies[e].vivo) continue;
                if (enemies[e].habitacion != currentRoom) continue;
                if (enemies[e].fila == item.fila && enemies[e].columna == item.columna) {
                    enemies[e].frozenTimer = 1.0e9f;
                    item.active  = true;

                    break;
                }
            }
        } else if (item.tipo == BARREL) {
            if (item.active) continue;
            for (int e = 0; e < enemyCount; e++) {
                if (!enemies[e].vivo) continue;
                if (enemies[e].habitacion != currentRoom) continue;
                if (enemies[e].fila == item.fila && enemies[e].columna == item.columna) {
                    enemies[e].dirFila      = -enemies[e].dirFila;
                    enemies[e].dirColumna      = -enemies[e].dirColumna;
                    enemies[e].frozenTimer = 0.3f;
                    item.active      = true;
                    item.effectTimer = 0.8f;
                    break;
                }
            }
        }

    }
}

void actualizarTimersObjetos(Objeto inventorySlots[], int slotCount,
                      Jugador &player, Enemigo enemies[], int enemyCount,
                      float dt) {
    (void)enemies;
    (void)enemyCount;

    for (int s = 0; s < slotCount; s++) {
        Objeto &item = inventorySlots[s];
        if (item.tipo == NINGUNO) continue;

        if (item.tipo == SPEEDBOOST) {
            if (item.active) {

                item.effectTimer -= dt;
                if (item.effectTimer <= 0.0f) {
                    item.effectTimer = 0.0f;
                    item.active      = false;
                    player.velocidad     = 210.0f;
                    item.cooldownTimer = 5.0f;
                }
            } else if (item.cooldownTimer > 0.0f) {
                item.cooldownTimer -= dt;
                if (item.cooldownTimer < 0.0f) item.cooldownTimer = 0.0f;
            }
        } else if (item.tipo == BARREL && item.active) {
            item.effectTimer -= dt;
            if (item.effectTimer <= 0.0f) {
                item.effectTimer = 0.0f;
                item.active      = false;
            }
        }

    }
}

void dibujarObjetosSuelo(const Objeto inventorySlots[], int slotCount,
                    sf::RenderWindow &window, int currentRoom) {
    for (int s = 0; s < slotCount; s++) {
        const Objeto &item = inventorySlots[s];
        if (!item.enSuelo || item.tipo == NINGUNO) continue;
        if (item.habitacion != currentRoom)         continue;

        float wx = (float)(item.columna * TAM_TILE);
        float wy = (float)(item.fila * TAM_TILE);

        if (item.tipo == TRAP) {
            dibujarSpriteProp(window, TRAP_RECT, wx, wy);
        } else if (item.tipo == SPEEDBOOST) {
            dibujarSpriteProp(window, SPEED_RECT, wx, wy);
        } else if (item.tipo == BARREL) {
            dibujarSpriteProp(window, BARREL_RECT, wx, wy);
        }
    }
}
