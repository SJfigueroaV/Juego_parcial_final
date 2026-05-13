#include "../include/player.h"
#include <cmath>

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

void manejarEntrada(Jugador &p, float dt) {
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
    p.x += (float)dc / len * p.velocidad * dt;
    p.y += (float)dr / len * p.velocidad * dt;

    p.columna = (int)((p.x + TAM_TILE * 0.5f) / TAM_TILE);
    p.fila    = (int)((p.y + TAM_TILE * 0.5f) / TAM_TILE);
}

void dibujarJugador(const Jugador &p, sf::RenderWindow &ventana) {
    sf::RectangleShape rect(sf::Vector2f{(float)TAM_TILE * 0.6f, (float)TAM_TILE * 0.6f});
    rect.setFillColor(sf::Color(80, 180, 255));
    rect.setPosition(sf::Vector2f{p.x + TAM_TILE * 0.2f, p.y + TAM_TILE * 0.2f});
    ventana.draw(rect);
}
