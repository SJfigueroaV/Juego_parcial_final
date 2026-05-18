#include "../include/player.h"
#include <cmath>
#include <string>

static const float SPRITE_SIZE = (float)TAM_TILE * 0.65f;
static const float SPRITE_OFF  = (float)TAM_TILE * 0.175f;

static const int   SHEET_FRAME_W  = 32;
static const int   SHEET_FRAME_H  = 32;
static const int   IDLE_COLS      = 2;
static const int   IDLE_TOTAL     = 2;
static const int   WALK_COLS      = 4;
static const int   WALK_TOTAL     = 4;
static const float IDLE_FRAME_DUR = 0.28f;
static const float WALK_FRAME_DUR = 0.10f;

static const float PLAYER_DRAW_SIZE = (float)TAM_TILE * 2.5f;
static const float PLAYER_DRAW_OFF  = ((float)TAM_TILE - PLAYER_DRAW_SIZE) * 0.5f;

static sf::Texture g_idleTex;
static sf::Texture g_walkTex;

void cargarTexturaJugadorReposo(const std::string &path) {
    (void)g_idleTex.loadFromFile(path);
}
void cargarTexturaJugadorCaminando(const std::string &path) {
    (void)g_walkTex.loadFromFile(path);
}

void inicializarJugador(Jugador &p, int startRow, int startCol) {
    p.fila       = startRow;
    p.columna       = startCol;
    p.x         = (float)(startCol * TAM_TILE);
    p.y         = (float)(startRow * TAM_TILE);
    p.velocidad     = 210.0f;
    p.moviendose  = false;
    p.animFrame = 0;
    p.animTimer = 0.0f;
}

static bool colisionaConPared(const Habitacion &room, float px, float py) {
    int r1 = (int)((py + SPRITE_OFF)                   / TAM_TILE);
    int r2 = (int)((py + SPRITE_OFF + SPRITE_SIZE - 1) / TAM_TILE);
    int c1 = (int)((px + SPRITE_OFF)                   / TAM_TILE);
    int c2 = (int)((px + SPRITE_OFF + SPRITE_SIZE - 1) / TAM_TILE);
    return esPared(room, r1, c1) || esPared(room, r1, c2) ||
           esPared(room, r2, c1) || esPared(room, r2, c2);
}

void manejarEntrada(Jugador &p, const Habitacion &room, float dt) {
    int dr = 0, dc = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)    ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))     dr -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)  ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))     dr += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)  ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))     dc -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))     dc += 1;

    bool nowMoving = (dr != 0 || dc != 0);

    if (nowMoving != p.moviendose) {
        p.moviendose  = nowMoving;
        p.animFrame = 0;
        p.animTimer = 0.0f;
    }

    float frameDur = nowMoving ? WALK_FRAME_DUR : IDLE_FRAME_DUR;
    int   total    = nowMoving ? WALK_TOTAL     : IDLE_TOTAL;
    p.animTimer += dt;
    while (p.animTimer >= frameDur) {
        p.animTimer -= frameDur;
        p.animFrame = (p.animFrame + 1) % total;
    }

    if (!nowMoving) return;

    float len  = std::sqrt((float)(dr * dr + dc * dc));
    float fdx  = (float)dc / len;
    float fdy  = (float)dr / len;
    float dist = p.velocidad * dt;
    float newX = p.x + fdx * dist;
    if (!colisionaConPared(room, newX, p.y)) p.x = newX;
    float newY = p.y + fdy * dist;
    if (!colisionaConPared(room, p.x, newY)) p.y = newY;

    p.columna = (int)((p.x + TAM_TILE * 0.5f) / TAM_TILE);
    p.fila = (int)((p.y + TAM_TILE * 0.5f) / TAM_TILE);
}

void dibujarJugador(const Jugador &p, sf::RenderWindow &window) {
    int cols  = p.moviendose ? WALK_COLS  : IDLE_COLS;
    int total = p.moviendose ? WALK_TOTAL : IDLE_TOTAL;
    int idx   = p.animFrame % total;
    int fx    = (idx % cols) * SHEET_FRAME_W;
    int fy    = (idx / cols) * SHEET_FRAME_H;

    sf::Sprite sprite(p.moviendose ? g_walkTex : g_idleTex);
    sprite.setTextureRect(sf::IntRect({fx, fy}, {SHEET_FRAME_W, SHEET_FRAME_H}));

    float scaleX = PLAYER_DRAW_SIZE / (float)SHEET_FRAME_W;
    float scaleY = PLAYER_DRAW_SIZE / (float)SHEET_FRAME_H;
    sprite.setScale(sf::Vector2f(scaleX, scaleY));
    sprite.setPosition(sf::Vector2f(p.x + PLAYER_DRAW_OFF, p.y + PLAYER_DRAW_OFF));
    window.draw(sprite);
}
