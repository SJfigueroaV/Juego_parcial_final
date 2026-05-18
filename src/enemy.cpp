#include "../include/enemy.h"
#include <cstdlib>
#include <cmath>

static const float SP_SIZE = (float)TAM_TILE * 0.65f;
static const float SP_OFF  = (float)TAM_TILE * 0.175f;

static void dibujarPixelArt(sf::RenderWindow &window,
                         float x, float y,
                         const int art[7][7],
                         const sf::Color palette[4]) {
    float ps = SP_SIZE / 7.0f;
    sf::RectangleShape px(sf::Vector2f(ps + 0.5f, ps + 0.5f));
    for (int r = 0; r < 7; r++) {
        for (int c = 0; c < 7; c++) {
            int ci = art[r][c];
            if (ci == 0) continue;
            px.setFillColor(palette[ci - 1]);
            px.setPosition(sf::Vector2f(x + c * ps, y + r * ps));
            window.draw(px);
        }
    }
}

static const int PERSEGUIDOR_ART[7][7] = {
    {0,1,1,1,1,1,0},
    {1,2,2,2,2,2,1},
    {1,2,4,2,4,2,1},
    {1,2,2,2,2,2,1},
    {0,1,2,2,2,1,0},
    {0,0,3,0,3,0,0},
    {0,0,0,0,0,0,0},
};
static const sf::Color PERSEGUIDOR_PAL[4] = {
    sf::Color( 90, 10, 10),
    sf::Color(200, 40, 40),
    sf::Color(235,215,180),
    sf::Color( 20, 10, 10),
};

static const int PATRULLERO_ART[7][7] = {
    {0,0,1,1,1,0,0},
    {0,1,2,2,2,1,0},
    {1,2,4,2,4,2,1},
    {1,2,2,2,2,2,1},
    {1,2,2,2,2,2,1},
    {1,2,1,2,1,2,1},
    {0,1,0,1,0,1,0},
};
static const sf::Color PATRULLERO_PAL[4] = {
    sf::Color(150, 70,  0),
    sf::Color(230,140, 30),
    sf::Color(255,200, 80),
    sf::Color( 20, 10, 10),
};

static int sign(int x) {
    if (x > 0) return  1;
    if (x < 0) return -1;
    return 0;
}

static void inicializarEnemigo(Enemigo &e, TipoEnemigo type, int roomIndex,
                      int row, int col, int dirRow, int dirCol,
                      int moveInterval) {
    e.tipo         = type;
    e.habitacion    = roomIndex;
    e.fila          = row;
    e.columna          = col;
    e.x            = (float)(col * TAM_TILE);
    e.y            = (float)(row * TAM_TILE);
    e.vivo        = true;
    e.dirFila       = dirRow;
    e.dirColumna       = dirCol;
    e.intervaloMov = moveInterval;
    e.tiempoMov    = moveInterval;
    e.frozenTimer  = 0.0f;
    e.slowTimer    = 0.0f;
    e.boostTimer   = 0.0f;
    e.knockbackVX  = 0.0f;
    e.knockbackVY  = 0.0f;

    const float SP = (float)TAM_TILE * 0.65f;
    const float SO = (float)TAM_TILE * 0.175f;
    e.shape.setSize(sf::Vector2f{SP, SP});
    e.shape.setPosition(sf::Vector2f{e.x + SO, e.y + SO});

    if (type == PERSEGUIDOR) {
        e.shape.setFillColor(sf::Color(200, 50, 50));
    } else {
        e.shape.setFillColor(sf::Color(230, 140, 30));
    }
}

void inicializarEnemigos(Enemigo enemies[], int &count) {
    count = 0;
    // Two patrol enemies in room 0's north-south corridor (cols 12-13, rows 1-18)
    inicializarEnemigo(enemies[count++], PATRULLERO, 0,  1, 12,  1, 0, 4); // starts north, goes south
    inicializarEnemigo(enemies[count++], PATRULLERO, 0, 18, 13, -1, 0, 4); // starts south, goes north
    // Two fast chasers in room 4, bottom-left and top-right corners
    inicializarEnemigo(enemies[count++], PERSEGUIDOR, 4, 17,  3,  0,  1,  7); // col=4 (1-indexed)
    inicializarEnemigo(enemies[count++], PERSEGUIDOR, 4, 17, 13,  0,  1,  7); // col=14 (1-indexed)
    inicializarEnemigo(enemies[count++], PERSEGUIDOR, 4, 17, 22,  0, -1,  7); // col=23 (1-indexed)
    // Habitacion 5 (sala 5): two patrols side-to-side, two chasers at bottom corners
    inicializarEnemigo(enemies[count++], PATRULLERO, 5,  9,  1, 0,  1, 6); // left wall → right
    inicializarEnemigo(enemies[count++], PATRULLERO, 5,  9, 24, 0, -1, 6); // right wall → left
    inicializarEnemigo(enemies[count++], PERSEGUIDOR, 5, 17,  2, 0,  1,  9); // bottom-left
    inicializarEnemigo(enemies[count++], PERSEGUIDOR, 5, 17, 23, 0, -1,  9); // bottom-right
}

static bool hayBarrilEn(const int barrelRows[], const int barrelCols[], int barrelCount,
                       int row, int col) {
    for (int i = 0; i < barrelCount; i++) {
        if (barrelRows[i] == row && barrelCols[i] == col) return true;
    }
    return false;
}

static void actualizarPerseguidor(Enemigo &e, const Jugador &player, const Habitacion &room,
                         const int barrelRows[], const int barrelCols[], int barrelCount) {
    int dRow = player.fila - e.fila;
    int dCol = player.columna - e.columna;

    if (dRow == 0 && dCol == 0) return;

    int sr = sign(dRow);
    int sc = sign(dCol);

    bool diagOk = !esPared(room, e.fila + sr, e.columna + sc) &&
                  !hayBarrilEn(barrelRows, barrelCols, barrelCount, e.fila + sr, e.columna + sc);
    bool rowOk  = !esPared(room, e.fila + sr, e.columna) &&
                  !hayBarrilEn(barrelRows, barrelCols, barrelCount, e.fila + sr, e.columna);
    bool colOk  = !esPared(room, e.fila, e.columna + sc) &&
                  !hayBarrilEn(barrelRows, barrelCols, barrelCount, e.fila, e.columna + sc);

    int prevRow = e.fila;
    int prevCol = e.columna;

    if (sr != 0 && sc != 0 && diagOk) {
        e.fila += sr;
        e.columna += sc;
    } else if (rowOk) {
        if (std::abs(dRow) >= std::abs(dCol)) {
            e.fila += sr;
        } else if (colOk) {
            e.columna += sc;
        } else {
            e.fila += sr;
        }
    } else if (colOk) {
        e.columna += sc;
    }

    // If stuck due to a barrel, launch a physics knockback in opposite direction
    if (e.fila == prevRow && e.columna == prevCol) {
        bool blockedByBarrel =
            (sr != 0 && hayBarrilEn(barrelRows, barrelCols, barrelCount, e.fila + sr, e.columna)) ||
            (sc != 0 && hayBarrilEn(barrelRows, barrelCols, barrelCount, e.fila, e.columna + sc)) ||
            (sr != 0 && sc != 0 && hayBarrilEn(barrelRows, barrelCols, barrelCount, e.fila + sr, e.columna + sc));

        if (blockedByBarrel) {
            float bLen = (sr != 0 && sc != 0) ? 1.4142f : 1.0f;
            e.knockbackVX = (float)(-sc) / bLen * 280.0f;
            e.knockbackVY = (float)(-sr) / bLen * 280.0f;
        }
    }
}

static void actualizarPatrullero(Enemigo &e, const Habitacion &room,
                         const int barrelRows[], const int barrelCols[], int barrelCount) {
    int nextRow = e.fila + e.dirFila;
    int nextCol = e.columna + e.dirColumna;

    bool blocked = esPared(room, nextRow, nextCol) ||
                   hayBarrilEn(barrelRows, barrelCols, barrelCount, nextRow, nextCol);
    if (blocked) {
        e.dirFila = -e.dirFila;
        e.dirColumna = -e.dirColumna;
        nextRow  = e.fila + e.dirFila;
        nextCol  = e.columna + e.dirColumna;
    }

    if (!esPared(room, nextRow, nextCol) &&
        !hayBarrilEn(barrelRows, barrelCols, barrelCount, nextRow, nextCol)) {
        e.fila = nextRow;
        e.columna = nextCol;
    }
}

void actualizarEnemigos(Enemigo enemies[], int count, const Jugador &player,
                   const Habitacion &room, float dt, int currentRoom,
                   const int barrelRows[], const int barrelCols[], int barrelCount) {
    for (int i = 0; i < count; i++) {
        if (!enemies[i].vivo || enemies[i].habitacion != currentRoom) {
            continue;
        }

        // Defense 1: if barrel was placed on top of this enemy's grid tile, push off
        {
            bool onBarrel = false;
            for (int bi = 0; bi < barrelCount && !onBarrel; bi++) {
                if (barrelRows[bi] == enemies[i].fila && barrelCols[bi] == enemies[i].columna)
                    onBarrel = true;
            }
            if (onBarrel) {
                int pushDr[4] = {-1, 1,  0, 0};
                int pushDc[4] = { 0, 0, -1, 1};
                for (int d = 0; d < 4; d++) {
                    int nr = enemies[i].fila + pushDr[d];
                    int nc = enemies[i].columna + pushDc[d];
                    if (!esPared(room, nr, nc) &&
                        !hayBarrilEn(barrelRows, barrelCols, barrelCount, nr, nc)) {
                        enemies[i].fila = nr;
                        enemies[i].columna = nc;
                        enemies[i].x   = (float)(nc * TAM_TILE);
                        enemies[i].y   = (float)(nr * TAM_TILE);
                        break;
                    }
                }
            }
        }

        if (enemies[i].frozenTimer > 0.0f) {
            enemies[i].frozenTimer -= dt;
            if (enemies[i].frozenTimer < 0.0f) enemies[i].frozenTimer = 0.0f;
        } else {
            if (enemies[i].slowTimer > 0.0f) {
                enemies[i].slowTimer -= dt;
                if (enemies[i].slowTimer < 0.0f) enemies[i].slowTimer = 0.0f;
            }

            int effectiveInterval = enemies[i].intervaloMov;
            if (enemies[i].slowTimer > 0.0f) effectiveInterval *= 2;

            enemies[i].tiempoMov--;
            if (enemies[i].tiempoMov <= 0) {
                enemies[i].tiempoMov = effectiveInterval;
                if (enemies[i].tipo == PERSEGUIDOR) {
                    actualizarPerseguidor(enemies[i], player, room, barrelRows, barrelCols, barrelCount);
                } else {
                    actualizarPatrullero(enemies[i], room, barrelRows, barrelCols, barrelCount);
                }
            }
        }

        float knockMag = std::sqrt(enemies[i].knockbackVX * enemies[i].knockbackVX +
                                   enemies[i].knockbackVY * enemies[i].knockbackVY);

        if (knockMag > 4.0f) {
            // Physics knockback: exponential drag (~0.5% of speed remains after 1 second)
            float decay = std::pow(0.004f, dt);
            float nx    = enemies[i].x + enemies[i].knockbackVX * dt;
            float ny    = enemies[i].y + enemies[i].knockbackVY * dt;
            int   nCol  = (int)(nx / TAM_TILE);
            int   nRow  = (int)(ny / TAM_TILE);

            bool wallX = esPared(room, (int)(ny / TAM_TILE), nCol) ||
                         hayBarrilEn(barrelRows, barrelCols, barrelCount, (int)(ny / TAM_TILE), nCol);
            bool wallY = esPared(room, nRow, (int)(nx / TAM_TILE)) ||
                         hayBarrilEn(barrelRows, barrelCols, barrelCount, nRow, (int)(nx / TAM_TILE));

            if (!wallX) enemies[i].x = nx;
            else        enemies[i].knockbackVX = 0.0f;

            if (!wallY) enemies[i].y = ny;
            else        enemies[i].knockbackVY = 0.0f;

            enemies[i].knockbackVX *= decay;
            enemies[i].knockbackVY *= decay;

            // Keep grid position in sync with pixel position
            enemies[i].columna = (int)((enemies[i].x + (float)TAM_TILE * 0.5f) / TAM_TILE);
            enemies[i].fila = (int)((enemies[i].y + (float)TAM_TILE * 0.5f) / TAM_TILE);
        } else {
            enemies[i].knockbackVX = 0.0f;
            enemies[i].knockbackVY = 0.0f;

            float targetX    = (float)(enemies[i].columna * TAM_TILE);
            float targetY    = (float)(enemies[i].fila * TAM_TILE);
            float pixelSpeed = (float)TAM_TILE * 60.0f / (float)enemies[i].intervaloMov;

            float dx   = targetX - enemies[i].x;
            float dy   = targetY - enemies[i].y;
            float dist = std::sqrt(dx * dx + dy * dy);
            float step = pixelSpeed * dt;

            if (dist <= step) {
                enemies[i].x = targetX;
                enemies[i].y = targetY;
            } else {
                // Defense 2: don't let the pixel enter a barrel tile's pixel space
                float nx   = enemies[i].x + dx / dist * step;
                float ny   = enemies[i].y + dy / dist * step;
                int   nCol = (int)(nx / TAM_TILE);
                int   nRow = (int)(ny / TAM_TILE);
                if (!hayBarrilEn(barrelRows, barrelCols, barrelCount, nRow, nCol)) {
                    enemies[i].x = nx;
                    enemies[i].y = ny;
                }
            }
        }

        const float SO = (float)TAM_TILE * 0.175f;
        enemies[i].shape.setPosition(sf::Vector2f{enemies[i].x + SO, enemies[i].y + SO});
    }
}

void dibujarEnemigos(const Enemigo enemies[], int count,
                 sf::RenderWindow &window, int currentRoom) {
    for (int i = 0; i < count; i++) {
        if (!enemies[i].vivo || enemies[i].habitacion != currentRoom) continue;
        float x = enemies[i].x + SP_OFF;
        float y = enemies[i].y + SP_OFF;
        if (enemies[i].tipo == PERSEGUIDOR)
            dibujarPixelArt(window, x, y, PERSEGUIDOR_ART, PERSEGUIDOR_PAL);
        else
            dibujarPixelArt(window, x, y, PATRULLERO_ART, PATRULLERO_PAL);
    }
}

void aumentarDificultad(Enemigo enemies[], int count, int level) {
    (void)level;
    for (int i = 0; i < count; i++) {
        if (enemies[i].intervaloMov > 2) {
            enemies[i].intervaloMov--;
        }
    }
}
