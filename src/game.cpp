#include "../include/game.h"
#include "../include/items.h"
#include <optional>
#include <string>
#include <cstdio>
#include <cstring>
#include <cmath>

bool verificarVictoria(int currentRoom) {
    return currentRoom == 3;
}

bool verificarDerrota(const Jugador &p, const Enemigo enemies[], int count, int currentRoom) {
    for (int i = 0; i < count; i++) {
        if (!enemies[i].vivo || enemies[i].habitacion != currentRoom) continue;
        if (enemies[i].frozenTimer > 0.0f) continue;
        float dx = enemies[i].x - p.x;
        float dy = enemies[i].y - p.y;
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if (dx < (float)TAM_TILE * 0.5f && dy < (float)TAM_TILE * 0.5f) {
            return true;
        }
    }
    return false;
}

static void inicializarLlaveMundo(Objeto &item, int row, int col) {
    std::strncpy(item.nombre, "Llave", sizeof(item.nombre));
    item.tipo        = LLAVE;
    item.placeable   = false;
    item.singleUse   = true;
    item.fila         = row;
    item.columna         = col;
    item.enSuelo   = false;
    item.active      = false;
    item.effectTimer = 0.0f;
}

static void obtenerPosicionEntrada(const Habitacion &room, int fromRoom,
                              int &outRow, int &outCol) {
    for (int d = 0; d < room.numPuertas; d++) {
        if (room.puertas[d].aHabitacion == fromRoom) {
            int r = room.puertas[d].fila;
            int c = room.puertas[d].columna;
            if      (r == 0)           { outRow = 1;          outCol = c; }
            else if (r == MAX_FILAS-1)  { outRow = MAX_FILAS-3; outCol = c; }
            else if (c == 0)           { outRow = r; outCol = 2; }
            else if (c == MAX_COLUMNAS-1)  { outRow = r; outCol = MAX_COLUMNAS-3; }
            else if (r < MAX_FILAS/2)   { outRow = r+1; outCol = c; }
            else                       { outRow = r-1; outCol = c; }
            return;
        }
    }
    outRow = MAX_FILAS / 2;
    outCol = MAX_COLUMNAS / 2;
}

static void dibujarObjetoMundo(const Objeto &item, sf::RenderWindow &window) {
    if (item.tipo == NINGUNO) return;

    if (item.tipo == COPA) {
        const sf::Texture *tex = obtenerTexturaPuerta();
        if (tex) {
            sf::Sprite spr(*tex);
            spr.setTextureRect(sf::IntRect({160, 64}, {16, 16}));
            const float KS = (float)TAM_TILE / 16.0f;
            spr.setScale(sf::Vector2f(KS, KS));
            spr.setPosition(sf::Vector2f(
                (float)(item.columna * TAM_TILE),
                (float)(item.fila * TAM_TILE)));
            window.draw(spr);
        } else {
            sf::RectangleShape shape(sf::Vector2f{
                (float)(TAM_TILE / 2), (float)(TAM_TILE / 2)
            });
            shape.setPosition(sf::Vector2f{
                (float)(item.columna * TAM_TILE + TAM_TILE / 4),
                (float)(item.fila * TAM_TILE + TAM_TILE / 4)
            });
            shape.setFillColor(sf::Color(255, 215, 0));
            window.draw(shape);
        }
        return;
    }

    const sf::Texture *keyTex = obtenerTexturaLlave();
    if (keyTex) {
        sf::Sprite keySprite(*keyTex);
        keySprite.setTextureRect(sf::IntRect({0, 0}, {16, 16}));
        const float KS = (float)TAM_TILE / 16.0f;
        keySprite.setScale(sf::Vector2f(KS, KS));
        keySprite.setPosition(sf::Vector2f(
            (float)(item.columna * TAM_TILE),
            (float)(item.fila * TAM_TILE)));
        window.draw(keySprite);
    } else {
        sf::RectangleShape shape(sf::Vector2f{
            (float)(TAM_TILE / 2), (float)(TAM_TILE / 2)
        });
        shape.setPosition(sf::Vector2f{
            (float)(item.columna * TAM_TILE + TAM_TILE / 4),
            (float)(item.fila * TAM_TILE + TAM_TILE / 4)
        });
        shape.setFillColor(sf::Color(255, 215, 0));
        window.draw(shape);
    }
}

static void dibujarGameOver(sf::RenderWindow &window, const sf::Font &font,
                          int level, int bestLevel) {
    sf::RectangleShape bg(sf::Vector2f{(float)WINDOW_W, (float)WINDOW_H});
    bg.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(bg);

    const float CX = (float)WINDOW_W / 2.0f;
    const float CY = (float)WINDOW_H / 2.0f;

    sf::Text title(font, "GAME OVER", 48);
    title.setFillColor(sf::Color::Red);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(sf::Vector2f{tb.size.x / 2.0f, tb.size.y / 2.0f});
    title.setPosition(sf::Vector2f{CX, CY - 110.0f});
    window.draw(title);

    char buf1[64];
    std::snprintf(buf1, sizeof(buf1), "Llegaste al nivel: %d", level);
    sf::Text t1(font, buf1, 22);
    t1.setFillColor(sf::Color::White);
    sf::FloatRect r1 = t1.getLocalBounds();
    t1.setOrigin(sf::Vector2f{r1.size.x / 2.0f, 0.0f});
    t1.setPosition(sf::Vector2f{CX, CY - 30.0f});
    window.draw(t1);

    char buf2[64];
    std::snprintf(buf2, sizeof(buf2), "Tu record: %d", bestLevel);
    sf::Text t2(font, buf2, 22);
    t2.setFillColor(sf::Color(255, 215, 0));
    sf::FloatRect r2 = t2.getLocalBounds();
    t2.setOrigin(sf::Vector2f{r2.size.x / 2.0f, 0.0f});
    t2.setPosition(sf::Vector2f{CX, CY + 10.0f});
    window.draw(t2);

    sf::Text t3(font, "Presiona R para reiniciar", 18);
    t3.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect r3 = t3.getLocalBounds();
    t3.setOrigin(sf::Vector2f{r3.size.x / 2.0f, 0.0f});
    t3.setPosition(sf::Vector2f{CX, CY + 60.0f});
    window.draw(t3);
}

static void dibujarOverlay(sf::RenderWindow &window, const sf::Font &font,
                        const char *message, sf::Color color) {
    sf::RectangleShape bg(sf::Vector2f{(float)WINDOW_W, (float)WINDOW_H});
    bg.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(bg);

    sf::Text title(font, message, 42);
    title.setFillColor(color);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(sf::Vector2f{tb.size.x / 2.0f, tb.size.y / 2.0f});
    title.setPosition(sf::Vector2f{(float)WINDOW_W / 2.0f, (float)WINDOW_H / 2.0f - 30.0f});
    window.draw(title);

    sf::Text hint(font, "R: reiniciar     ESC: salir", 18);
    hint.setFillColor(sf::Color::White);
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(sf::Vector2f{hb.size.x / 2.0f, 0.0f});
    hint.setPosition(sf::Vector2f{(float)WINDOW_W / 2.0f, (float)WINDOW_H / 2.0f + 20.0f});
    window.draw(hint);
}

static sf::View crearVistaEscalada(unsigned int w, unsigned int h) {
    float sx = (float)w / (float)WINDOW_W;
    float sy = (float)h / (float)WINDOW_H;
    float s  = sx < sy ? sx : sy;
    s = std::floor(s * 2.0f) / 2.0f;
    if (s < 0.5f) s = 0.5f;
    float vpW = std::floor((float)WINDOW_W * s);
    float vpH = std::floor((float)WINDOW_H * s);
    float ox  = std::floor(((float)w - vpW) * 0.5f);
    float oy  = std::floor(((float)h - vpH) * 0.5f);
    sf::View v(sf::FloatRect(sf::Vector2f{0.f, 0.f},
                             sf::Vector2f{(float)WINDOW_W, (float)WINDOW_H}));
    v.setViewport(sf::FloatRect(
        sf::Vector2f{ox / (float)w, oy / (float)h},
        sf::Vector2f{vpW / (float)w, vpH / (float)h}
    ));
    return v;
}

void ejecutarJuego() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float sx0 = (float)desktop.size.x / (float)WINDOW_W;
    float sy0 = (float)desktop.size.y / (float)WINDOW_H;
    float s0  = sx0 < sy0 ? sx0 : sy0;
    unsigned int winW = (unsigned int)((float)WINDOW_W * s0);
    unsigned int winH = (unsigned int)((float)WINDOW_H * s0);

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{winW, winH}),
        "Dungeon Crawler",
        sf::Style::Default
    );
    window.setFramerateLimit(60);
    window.setView(crearVistaEscalada(winW, winH));

    sf::Font font;
    bool fontLoaded = font.openFromFile(
        "/usr/share/fonts/noto/NotoSans-Regular.ttf");
    if (!fontLoaded) {
        fontLoaded = font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");
    }
    if (!fontLoaded) {
        fontLoaded = font.openFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }

    cargarTileset(    "../include/sprites/Dungeon_Tileset_v2.png");
    cargarTexturaPuerta("../include/sprites/Dungeon_item_props_v2.png");
    cargarTexturaLlave( "../include/sprites/keys_g.png");
    cargarTexturaJugadorReposo("../include/sprites/idle.png");
    cargarTexturaJugadorCaminando("../include/sprites/walk.png");

    bool keepRunning = true;

    while (window.isOpen() && keepRunning) {
        Habitacion rooms[MAX_HABITACIONES];
        int  roomCount = 0;
        inicializarHabitaciones(rooms, roomCount);

        int bestLevel = 0;
        Jugador    player;
        Enemigo     enemies[MAX_ENEMIES];
        Objeto      worldItems[MAX_HABITACIONES];
        Inventario inv;
        int       enemyCount = 0;

        int   currentRoom = 0;
        int   heldKeyId   = -1;
        bool  heldCopa    = false;
        int   level       = 0;
        float spawnTimer  = 1.5f;

        inicializarJugador(player, 9, 5);
        inicializarEnemigos(enemies, enemyCount);
        inicializarInventario(inv);

        Cofre chests[MAX_CHESTS];
        int   chestCount = 0;
        // Chests in rooms[1] (mini-sala)
        chests[0].fila = 10; chests[0].columna = 11; chests[0].habitacion = 1;
        chests[0].opened = false; chests[0].itemType = TRAP;
        chests[1].fila = 10; chests[1].columna = 14; chests[1].habitacion = 1;
        chests[1].opened = false; chests[1].itemType = SPEEDBOOST;
        // Chests in rooms[2] (maze open area)
        chests[2].fila = 18; chests[2].columna = 14; chests[2].habitacion = 2;
        chests[2].opened = false; chests[2].itemType = TRAP;
        chests[3].fila = 18; chests[3].columna = 15; chests[3].habitacion = 2;
        chests[3].opened = false; chests[3].itemType = BARREL;
        // Test chest: barrel debug (rooms[0], col=6, row=9)
        chests[4].fila = 9; chests[4].columna = 6; chests[4].habitacion = 0;
        chests[4].opened = false; chests[4].itemType = BARREL;
        chestCount = 5;

        Objeto floorItems[MAX_SLOTS];
        for (int fi = 0; fi < MAX_SLOTS; fi++) {
            floorItems[fi].tipo      = NINGUNO;
            floorItems[fi].enSuelo = true;
            floorItems[fi].active    = false;
            floorItems[fi].effectTimer   = 0.0f;
            floorItems[fi].cooldownTimer = 0.0f;
            floorItems[fi].fila       = 0;
            floorItems[fi].columna       = 0;
            floorItems[fi].habitacion = 0;
        }

        inicializarLlaveMundo(worldItems[0],  9, 19);
        inicializarLlaveMundo(worldItems[1],  8,  8);
        inicializarLlaveMundo(worldItems[2], 17, 23);
        worldItems[3].tipo        = NINGUNO;
        worldItems[3].enSuelo   = false;
        worldItems[3].active      = false;
        worldItems[3].effectTimer = 0.0f;
        worldItems[4].tipo        = NINGUNO;
        worldItems[4].enSuelo   = false;
        worldItems[4].active      = false;
        worldItems[4].effectTimer = 0.0f;
        // COPA in rooms[5]
        std::strncpy(worldItems[5].nombre, "Copa", sizeof(worldItems[5].nombre));
        worldItems[5].tipo          = COPA;
        worldItems[5].placeable     = false;
        worldItems[5].singleUse     = true;
        worldItems[5].fila           = 16;
        worldItems[5].columna           = 13;
        worldItems[5].habitacion     = 5;
        worldItems[5].enSuelo     = false;
        worldItems[5].active        = false;
        worldItems[5].effectTimer   = 0.0f;
        worldItems[5].cooldownTimer = 0.0f;

        EstadoJuego state            = JUGANDO;
        bool      restartRequested = false;
        bool      prevOnDoor       = false;
        sf::Clock clock;

        while (window.isOpen() && !restartRequested) {
            float dt = clock.restart().asSeconds();

            while (std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                    keepRunning = false;
                }
                if (event->is<sf::Event::Resized>()) {
                    const sf::Event::Resized *rs = event->getIf<sf::Event::Resized>();
                    window.setView(crearVistaEscalada(rs->size.x, rs->size.y));
                }
                if (event->is<sf::Event::KeyPressed>()) {
                    const sf::Event::KeyPressed *kp =
                        event->getIf<sf::Event::KeyPressed>();
                    switch (kp->code) {
                    case sf::Keyboard::Key::Escape:
                        window.close();
                        keepRunning = false;
                        break;
                    case sf::Keyboard::Key::E:
                        if (state == JUGANDO) {
                            bool interacted = false;
                            int  drs[5] = {0, -1, 1,  0, 0};
                            int  dcs[5] = {0,  0, 0, -1, 1};
                            Habitacion &cur   = rooms[currentRoom];

                            // 1. Pick up own floor item
                            for (int s = 0; s < MAX_SLOTS && !interacted; s++) {
                                if (floorItems[s].tipo != NINGUNO &&
                                    floorItems[s].habitacion == currentRoom &&
                                    floorItems[s].fila == player.fila &&
                                    floorItems[s].columna == player.columna) {
                                    if (agregarObjeto(inv, floorItems[s].tipo)) {
                                        floorItems[s].tipo = NINGUNO;
                                        interacted = true;
                                    }
                                }
                            }

                            // 5. Open chest
                            if (!interacted) {
                                for (int d = 0; d < 5 && !interacted; d++) {
                                    int nr = player.fila + drs[d];
                                    int nc = player.columna + dcs[d];
                                    if (nr < 0 || nr >= MAX_FILAS ||
                                        nc < 0 || nc >= MAX_COLUMNAS) continue;
                                    if (cur.cuadricula[nc][nr] == T_CHEST) {
                                        for (int ci = 0; ci < chestCount; ci++) {
                                            if (chests[ci].habitacion == currentRoom &&
                                                chests[ci].fila == nr &&
                                                chests[ci].columna == nc &&
                                                !chests[ci].opened) {
                                                if (agregarObjeto(inv, chests[ci].itemType)) {
                                                    chests[ci].opened  = true;
                                                    cur.cuadricula[nc][nr] = T_FD;
                                                }
                                                interacted = true;
                                            }
                                        }
                                    }
                                }
                            }

                            // 6. Activate ability or place item (only if no world interaction)
                            if (!interacted) {
                                Objeto &sel = inv.espacios[inv.espacioSeleccionado];
                                if (sel.tipo == SPEEDBOOST && !sel.enSuelo &&
                                    !sel.active && sel.cooldownTimer <= 0.0f) {
                                    player.velocidad    = 210.0f * 1.5f;
                                    sel.active      = true;
                                    sel.effectTimer = 1.5f;
                                } else if (sel.tipo != NINGUNO && sel.placeable) {
                                    for (int fi = 0; fi < MAX_SLOTS; fi++) {
                                        if (floorItems[fi].tipo == NINGUNO) {
                                            floorItems[fi]             = sel;
                                            floorItems[fi].enSuelo   = true;
                                            floorItems[fi].fila         = player.fila;
                                            floorItems[fi].columna         = player.columna;
                                            floorItems[fi].habitacion   = currentRoom;
                                            floorItems[fi].active      = false;
                                            floorItems[fi].effectTimer = 0.0f;
                                            quitarObjeto(inv, inv.espacioSeleccionado);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    case sf::Keyboard::Key::P:
                        if (state == JUGANDO) {
                            Objeto &psel = inv.espacios[inv.espacioSeleccionado];
                            if (psel.tipo != NINGUNO && psel.placeable) {
                                for (int fi = 0; fi < MAX_SLOTS; fi++) {
                                    if (floorItems[fi].tipo == NINGUNO) {
                                        floorItems[fi]             = psel;
                                        floorItems[fi].enSuelo   = true;
                                        floorItems[fi].fila         = player.fila;
                                        floorItems[fi].columna         = player.columna;
                                        floorItems[fi].habitacion   = currentRoom;
                                        floorItems[fi].active      = false;
                                        floorItems[fi].effectTimer = 0.0f;
                                        quitarObjeto(inv, inv.espacioSeleccionado);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    case sf::Keyboard::Key::Num1: seleccionarSlot(inv, 0); break;
                    case sf::Keyboard::Key::Num2: seleccionarSlot(inv, 1); break;
                    case sf::Keyboard::Key::Num3: seleccionarSlot(inv, 2); break;
                    case sf::Keyboard::Key::Num4: seleccionarSlot(inv, 3); break;
                    case sf::Keyboard::Key::Num5: seleccionarSlot(inv, 4); break;
                    case sf::Keyboard::Key::Q:
                        if (state == JUGANDO) {
                            Objeto &sel = inv.espacios[inv.espacioSeleccionado];
                            bool canDrop = sel.tipo != NINGUNO && sel.tipo != LLAVE &&
                                          !(sel.tipo == SPEEDBOOST && sel.cooldownTimer > 0.0f);
                            if (canDrop) {
                                for (int fi = 0; fi < MAX_SLOTS; fi++) {
                                    if (floorItems[fi].tipo == NINGUNO) {
                                        if (sel.tipo == SPEEDBOOST) {
                                            player.velocidad      = 210.0f;
                                            sel.cooldownTimer = 0.0f;
                                        }
                                        floorItems[fi]             = sel;
                                        floorItems[fi].enSuelo   = true;
                                        floorItems[fi].fila         = player.fila;
                                        floorItems[fi].columna         = player.columna;
                                        floorItems[fi].habitacion   = currentRoom;
                                        floorItems[fi].active      = false;
                                        floorItems[fi].effectTimer = 0.0f;
                                        quitarObjeto(inv, inv.espacioSeleccionado);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    case sf::Keyboard::Key::R:
                        if (state != JUGANDO) {
                            restartRequested = true;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }

            if (state == JUGANDO) {
                if (spawnTimer > 0.0f) {
                    spawnTimer -= dt;
                    if (spawnTimer < 0.0f) { spawnTimer = 0.0f; }
                }

                manejarEntrada(player, rooms[currentRoom], dt);

                // Auto-pickup world key when player walks over it
                {
                    if (worldItems[currentRoom].tipo == LLAVE) {
                        int prevCount = inv.numObjetos;
                        verificarRecoleccion(inv, &worldItems[currentRoom], 1, player);
                        if (inv.numObjetos != prevCount) {
                            for (int i = 0; i < MAX_SLOTS; i++) {
                                if (inv.espacios[i].tipo == LLAVE) {
                                    if      (currentRoom == 0) heldKeyId = 1;
                                    else if (currentRoom == 1) heldKeyId = 4;
                                    else if (currentRoom == 2) heldKeyId = 2;
                                    quitarObjeto(inv, i);
                                    break;
                                }
                            }
                        }
                    }
                }
                // Auto-pickup copa by pixel proximity (holder is solid, can't step on it)
                {
                    if (!heldCopa && worldItems[currentRoom].tipo == COPA) {
                        Objeto &copa  = worldItems[currentRoom];
                        float px    = player.x + TAM_TILE * 0.5f;
                        float py    = player.y + TAM_TILE * 0.5f;
                        float cx    = copa.columna * TAM_TILE + TAM_TILE * 0.5f;
                        float cy    = copa.fila * TAM_TILE + TAM_TILE * 0.5f;
                        float ddx   = px - cx;
                        float ddy   = py - cy;
                        float dist2 = ddx * ddx + ddy * ddy;
                        float limit = (float)(TAM_TILE * TAM_TILE) * 2.0f;
                        if (dist2 <= limit) {
                            copa.tipo = NINGUNO;
                            heldCopa  = true;
                        }
                    }
                }

                // Auto-unlock locked door when player is adjacent and has the key
                {
                    Habitacion &cur2 = rooms[currentRoom];
                    int drs2[5] = {0, -1, 1,  0, 0};
                    int dcs2[5] = {0,  0, 0, -1, 1};
                    bool unlocked = false;
                    // Copa: opens the special blue door (T_SDOOR) → victory
                    if (heldCopa) {
                        for (int d = 0; d < 5 && !unlocked; d++) {
                            int nr = player.fila + drs2[d];
                            int nc = player.columna + dcs2[d];
                            if (nr < 0 || nr >= MAX_FILAS || nc < 0 || nc >= MAX_COLUMNAS) continue;
                            if (cur2.cuadricula[nc][nr] == T_SDOOR) {
                                state    = VICTORIA;
                                unlocked = true;
                            }
                        }
                    }
                    if (heldKeyId == 2) {
                        for (int d = 0; d < 5 && !unlocked; d++) {
                            int nr = player.fila + drs2[d];
                            int nc = player.columna + dcs2[d];
                            if (nr < 0 || nr >= MAX_FILAS || nc < 0 || nc >= MAX_COLUMNAS) continue;
                            if (cur2.cuadricula[nc][nr] == T_LDOOR) {
                                for (int r = 0; r < MAX_FILAS; r++) {
                                    for (int c = 0; c < MAX_COLUMNAS; c++) {
                                        if (cur2.cuadricula[c][r] == T_LDOOR)
                                            cur2.cuadricula[c][r] = T_F;
                                    }
                                }
                                heldKeyId = -1;
                                unlocked = true;
                            }
                        }
                    }
                    if (!unlocked && heldKeyId >= 0) {
                        for (int d = 0; d < 5 && !unlocked; d++) {
                            int nr = player.fila + drs2[d];
                            int nc = player.columna + dcs2[d];
                            if (nr < 0 || nr >= MAX_FILAS || nc < 0 || nc >= MAX_COLUMNAS) continue;
                            if (cur2.cuadricula[nc][nr] == T_DLOCKED) {
                                const Puerta *adjDoor = obtenerPuertaEn(cur2, nr, nc);
                                if (adjDoor != nullptr && heldKeyId == adjDoor->aHabitacion) {
                                    int targetRoom = adjDoor->aHabitacion;
                                    int doorRow    = nr;
                                    for (int cc = 0; cc < MAX_COLUMNAS; cc++) {
                                        if (cur2.cuadricula[cc][doorRow] == T_DLOCKED)
                                            cur2.cuadricula[cc][doorRow] = T_D;
                                    }
                                    for (int di = 0; di < cur2.numPuertas; di++) {
                                        if (cur2.puertas[di].aHabitacion == targetRoom)
                                            cur2.puertas[di].bloqueada = false;
                                    }
                                    heldKeyId = -1;
                                    unlocked = true;
                                }
                            }
                        }
                    }
                }

                int barrelRows[MAX_SLOTS], barrelCols[MAX_SLOTS], barrelCount = 0;
                for (int fi = 0; fi < MAX_SLOTS; fi++) {
                    if (floorItems[fi].tipo == BARREL) {
                        barrelRows[barrelCount] = floorItems[fi].fila;
                        barrelCols[barrelCount] = floorItems[fi].columna;
                        barrelCount++;
                    }
                }
                actualizarEnemigos(enemies, enemyCount, player,
                              rooms[currentRoom], dt, currentRoom,
                              barrelRows, barrelCols, barrelCount);
                verificarEfectosObjetos(floorItems, MAX_SLOTS, player, enemies, enemyCount, currentRoom);
                actualizarTimersObjetos(inv.espacios,  MAX_SLOTS, player, enemies, enemyCount, dt);
                actualizarTimersObjetos(floorItems, MAX_SLOTS, player, enemies, enemyCount, dt);

                const Puerta *door = obtenerPuertaEn(rooms[currentRoom],
                                             player.fila, player.columna);
                if (door != nullptr && !prevOnDoor) {
                    bool canEnter = !door->bloqueada || (heldKeyId == door->aHabitacion);
                    if (canEnter) {
                        if (door->aHabitacion >= MAX_HABITACIONES) {
                            state = VICTORIA;
                        } else {
                            int prevRoom = currentRoom;
                            currentRoom  = door->aHabitacion;
                            if (door->bloqueada) {
                                heldKeyId = -1;
                                level++;
                                aumentarDificultad(enemies, enemyCount, level);
                            }
                            int entRow = 0;
                            int entCol = 0;
                            obtenerPosicionEntrada(rooms[currentRoom], prevRoom,
                                             entRow, entCol);
                            inicializarJugador(player, entRow, entCol);
                            spawnTimer = 1.5f;
                        }
                    }
                }
                prevOnDoor = (door != nullptr);

                if (spawnTimer <= 0.0f &&
                    verificarDerrota(player, enemies, enemyCount, currentRoom)) {
                    state = DERROTA;
                    if (level > bestLevel) {
                        bestLevel = level;
                    }
                }
            }

            window.clear(sf::Color(10, 10, 10));
            dibujarHabitacion(rooms[currentRoom], window, player, currentRoom);
            if (!(currentRoom == 5 && worldItems[5].tipo == COPA)) {
                dibujarObjetoMundo(worldItems[currentRoom], window);
            }
            if (currentRoom == 5 && worldItems[5].tipo == COPA) {
                const sf::Texture *copaTex = obtenerTexturaPuerta();
                if (copaTex) {
                    const float KS = (float)TAM_TILE / 16.0f;
                    sf::Sprite copaSpr(*copaTex);
                    copaSpr.setTextureRect(sf::IntRect(sf::Vector2i{160, 64}, sf::Vector2i{16, 16}));
                    copaSpr.setScale(sf::Vector2f(KS, KS));
                    copaSpr.setPosition(sf::Vector2f(
                        13.0f * TAM_TILE + 2.0f,
                        17.0f * TAM_TILE - TAM_TILE * 0.6f + 0.9f));
                    window.draw(copaSpr);
                }
            }
            dibujarObjetosSuelo(floorItems, MAX_SLOTS, window, currentRoom);
            dibujarEnemigos(enemies, enemyCount, window, currentRoom);
            dibujarJugador(player, window);

            // Draw selected item sprite to the left of the player
            {
                const Objeto &sel = inv.espacios[inv.espacioSeleccionado];
                float ix = player.x - (float)TAM_TILE * 0.65f;
                float iy = player.y + (float)TAM_TILE * 0.3f;
                const float ITEM_SCALE = (float)TAM_TILE / 16.0f * 0.6f;
                const float ITEM_SIZE  = 16.0f * ITEM_SCALE;
                float cx = ix + ((float)TAM_TILE - ITEM_SIZE) * 0.5f;
                float cy = iy + ((float)TAM_TILE - ITEM_SIZE) * 0.5f;
                if (sel.tipo == LLAVE) {
                    const sf::Texture *kTex = obtenerTexturaLlave();
                    if (kTex) {
                        sf::Sprite kSpr(*kTex);
                        kSpr.setTextureRect(sf::IntRect({0, 0}, {16, 16}));
                        kSpr.setScale(sf::Vector2f{ITEM_SCALE, ITEM_SCALE});
                        kSpr.setPosition(sf::Vector2f{cx, cy});
                        window.draw(kSpr);
                    }
                } else {
                    const sf::Texture *gTex = obtenerTexturaPuerta();
                    sf::IntRect rect({0, 0}, {16, 16});
                    bool hasSpr = true;
                    if      (sel.tipo == TRAP)       rect = sf::IntRect(sf::Vector2i{  0, 64}, sf::Vector2i{16, 16});
                    else if (sel.tipo == SPEEDBOOST) rect = sf::IntRect(sf::Vector2i{160, 16}, sf::Vector2i{16, 16});
                    else if (sel.tipo == BARREL)     rect = sf::IntRect(sf::Vector2i{ 64, 64}, sf::Vector2i{16, 16});
                    else hasSpr = false;
                    if (hasSpr && gTex) {
                        sf::Sprite iSpr(*gTex);
                        iSpr.setTextureRect(rect);
                        iSpr.setScale(sf::Vector2f{ITEM_SCALE, ITEM_SCALE});
                        iSpr.setPosition(sf::Vector2f{cx, cy});
                        window.draw(iSpr);
                    }
                }
            }

            if (fontLoaded) {
                dibujarInventario(inv, window, font);

                if (state == VICTORIA) {
                    dibujarOverlay(window, font, "VICTORIA!", sf::Color::Yellow);
                } else if (state == DERROTA) {
                    dibujarGameOver(window, font, level, bestLevel);
                }
            }

            window.display();
        }
    }
}
