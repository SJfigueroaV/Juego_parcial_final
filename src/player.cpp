#include "../include/player.h"
#include <cmath>

static const float SPRITE_SIZE = (float)TAM_TILE * 0.6f;
static const float SPRITE_OFF  = (float)TAM_TILE * 0.2f;

void inicializarJugador(Jugador &p, int fila, int columna) {
    p.fila        = fila;
    p.columna     = columna;
    p.x           = (float)(columna * TAM_TILE);
    p.y           = (float)(fila * TAM_TILE);
    p.velocidad   = 210.0f;
    p.moviendose  = false;
    p.animFrame   = 0;
    p.animTimer   = 0.0f;
}

static bool colisionaConPared(const Habitacion &habitacion, float px, float py) {
    int f1 = (int)((py + SPRITE_OFF)                   / TAM_TILE);
    int f2 = (int)((py + SPRITE_OFF + SPRITE_SIZE - 1) / TAM_TILE);
    int c1 = (int)((px + SPRITE_OFF)                   / TAM_TILE);
    int c2 = (int)((px + SPRITE_OFF + SPRITE_SIZE - 1) / TAM_TILE);
    return esPared(habitacion, f1, c1) || esPared(habitacion, f1, c2) ||
           esPared(habitacion, f2, c1) || esPared(habitacion, f2, c2);
}

void manejarEntrada(Jugador &p, const Habitacion &habitacion, float dt) {
    int dr = 0;
    int dc = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dr -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dr += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dc -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dc += 1;

    p.moviendose = (dr != 0 || dc != 0);
    if (!p.moviendose) return;

    float len = std::sqrt((float)(dr * dr + dc * dc));
    float fdx = (float)dc / len;
    float fdy = (float)dr / len;
    float dist = p.velocidad * dt;

    float newX = p.x + fdx * dist;
    if (!colisionaConPared(habitacion, newX, p.y)) p.x = newX;
    float newY = p.y + fdy * dist;
    if (!colisionaConPared(habitacion, p.x, newY)) p.y = newY;

    p.columna = (int)((p.x + TAM_TILE * 0.5f) / TAM_TILE);
    p.fila    = (int)((p.y + TAM_TILE * 0.5f) / TAM_TILE);
}

void dibujarJugador(const Jugador &p, sf::RenderWindow &ventana) {
    sf::RectangleShape rect(sf::Vector2f{SPRITE_SIZE, SPRITE_SIZE});
    rect.setFillColor(sf::Color(80, 180, 255));
    rect.setPosition(sf::Vector2f{p.x + SPRITE_OFF, p.y + SPRITE_OFF});
    ventana.draw(rect);
}
