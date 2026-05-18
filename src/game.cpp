#include "../include/game.h"
#include "../include/map.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/inventory.h"
#include "../include/items.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <cstring>

bool verificarVictoria(int habitacionActual) {
    return habitacionActual == 3;
}

bool verificarDerrota(const Jugador &p, const Enemigo enemigos[], int numEnemigos,
                     int habitacionActual) {
    for (int i = 0; i < numEnemigos; i++) {
        if (!enemigos[i].vivo || enemigos[i].habitacion != habitacionActual) continue;
        float dx = enemigos[i].x - p.x;
        float dy = enemigos[i].y - p.y;
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if (dx < (float)TAM_TILE * 0.5f && dy < (float)TAM_TILE * 0.5f) {
            return true;
        }
    }
    return false;
}

static void inicializarLlaveMundo(Objeto &obj, int habitacion, int fila, int columna) {
    obj.tipo       = LLAVE;
    std::strncpy(obj.nombre, "Llave", sizeof(obj.nombre));
    obj.fila       = fila;
    obj.columna    = columna;
    obj.habitacion = habitacion;
    obj.enSuelo    = true;
    obj.placeable  = false;
    obj.singleUse  = true;
    obj.active     = false;
    obj.effectTimer = 0.0f;
    obj.cooldownTimer = 0.0f;
}

static void posicionarEnEntrada(const Habitacion &habitacion, int desdeHabitacion,
                                Jugador &jugador) {
    for (int d = 0; d < habitacion.numPuertas; d++) {
        if (habitacion.puertas[d].aHabitacion == desdeHabitacion) {
            int f = habitacion.puertas[d].fila;
            int c = habitacion.puertas[d].columna;
            int nf = f;
            int nc = c;
            if      (f == 0)              nf = 1;
            else if (f == MAX_FILAS - 1)  nf = MAX_FILAS - 3;
            else if (c == 0)              nc = 2;
            else if (c == MAX_COLUMNAS-1) nc = MAX_COLUMNAS - 3;
            jugador.fila    = nf;
            jugador.columna = nc;
            jugador.x       = (float)(nc * TAM_TILE);
            jugador.y       = (float)(nf * TAM_TILE);
            return;
        }
    }
}

static void dibujarHUD(sf::RenderWindow &ventana, const Jugador &jugador,
                       const Inventario &inv, int habitacionActual) {
    // Barra HP arriba izquierda
    float barW = 160.0f;
    float barH = 14.0f;
    sf::RectangleShape fondoHp(sf::Vector2f{barW, barH});
    fondoHp.setPosition(sf::Vector2f{10.0f, 10.0f});
    fondoHp.setFillColor(sf::Color(40, 40, 40));
    ventana.draw(fondoHp);

    float pct = (float)jugador.hp / (float)jugador.maxHp;
    sf::RectangleShape hp(sf::Vector2f{barW * pct, barH});
    hp.setPosition(sf::Vector2f{10.0f, 10.0f});
    hp.setFillColor(sf::Color(200, 30, 30));
    ventana.draw(hp);

    // Indicador de habitación (esquina superior derecha, simulado con bloques)
    for (int i = 0; i < MAX_HABITACIONES; i++) {
        sf::RectangleShape b(sf::Vector2f{12.0f, 12.0f});
        b.setPosition(sf::Vector2f{
            (float)(MAX_COLUMNAS * TAM_TILE) - 16.0f * (MAX_HABITACIONES - i),
            10.0f
        });
        if (i == habitacionActual) {
            b.setFillColor(sf::Color(255, 215, 0));
        } else {
            b.setFillColor(sf::Color(60, 60, 60));
        }
        ventana.draw(b);
    }

    // Panel de inventario abajo
    const int DISPLAY_SLOTS = 3;
    float panelY = (float)(MAX_FILAS * TAM_TILE) - 50.0f;
    for (int i = 0; i < DISPLAY_SLOTS; i++) {
        sf::RectangleShape slot(sf::Vector2f{40.0f, 40.0f});
        slot.setPosition(sf::Vector2f{
            10.0f + (float)i * 48.0f,
            panelY
        });
        slot.setFillColor(sf::Color(30, 30, 30, 220));
        if (i == inv.espacioSeleccionado) {
            slot.setOutlineColor(sf::Color::Yellow);
            slot.setOutlineThickness(2.0f);
        } else {
            slot.setOutlineColor(sf::Color(120, 120, 120));
            slot.setOutlineThickness(1.0f);
        }
        ventana.draw(slot);

        // Indicador interno del objeto (color por tipo)
        const Objeto &obj = inv.espacios[i];
        if (obj.tipo != NINGUNO && !obj.enSuelo) {
            sf::RectangleShape inside(sf::Vector2f{20.0f, 20.0f});
            inside.setPosition(sf::Vector2f{
                20.0f + (float)i * 48.0f,
                panelY + 10.0f
            });
            if (obj.tipo == LLAVE)           inside.setFillColor(sf::Color(255, 215, 0));
            else if (obj.tipo == TRAP)       inside.setFillColor(sf::Color(120, 60, 200));
            else if (obj.tipo == BARREL)     inside.setFillColor(sf::Color(180, 100, 30));
            else if (obj.tipo == SPEEDBOOST) inside.setFillColor(sf::Color(80, 200, 80));
            ventana.draw(inside);
        }
    }
}

static void dibujarOverlayEstado(sf::RenderWindow &ventana, EstadoJuego estado) {
    if (estado == JUGANDO) return;
    sf::RectangleShape bg(sf::Vector2f{
        (float)(MAX_COLUMNAS * TAM_TILE), (float)(MAX_FILAS * TAM_TILE)
    });
    bg.setFillColor(sf::Color(0, 0, 0, 180));
    ventana.draw(bg);
    sf::RectangleShape centro(sf::Vector2f{(float)TAM_TILE * 6.0f, (float)TAM_TILE * 2.0f});
    centro.setPosition(sf::Vector2f{
        (float)(MAX_COLUMNAS * TAM_TILE) / 2.0f - TAM_TILE * 3.0f,
        (float)(MAX_FILAS * TAM_TILE) / 2.0f - TAM_TILE
    });
    if (estado == VICTORIA) {
        centro.setFillColor(sf::Color(220, 200, 30));
    } else {
        centro.setFillColor(sf::Color(180, 30, 30));
    }
    ventana.draw(centro);
}

void ejecutarJuego() {
    Habitacion habitaciones[MAX_HABITACIONES];
    int numHabitaciones = 0;
    inicializarHabitaciones(habitaciones, numHabitaciones);

    int habitacionActual = 0;

    Jugador jugador;
    inicializarJugador(jugador, 10, 13);

    Enemigo enemigos[MAX_ENEMIES];
    int numEnemigos = 0;
    inicializarEnemigos(enemigos, numEnemigos);

    Inventario inv;
    inicializarInventario(inv);

    Objeto objetosMundo[MAX_HABITACIONES];
    for (int i = 0; i < MAX_HABITACIONES; i++) objetosMundo[i].tipo = NINGUNO;
    inicializarLlaveMundo(objetosMundo[2], 2, 5, 12);

    // Items extra disponibles desde el principio: TRAP, SPEEDBOOST, BARREL en slots
    agregarObjeto(inv, TRAP);
    agregarObjeto(inv, SPEEDBOOST);
    agregarObjeto(inv, BARREL);

    sf::RenderWindow ventana(
        sf::VideoMode(sf::Vector2u{MAX_COLUMNAS * TAM_TILE, MAX_FILAS * TAM_TILE}),
        "Dungeon");
    ventana.setFramerateLimit(60);

    sf::Clock reloj;
    bool sobrePuertaAntes = false;
    bool tieneLlave        = false;
    EstadoJuego estado     = JUGANDO;

    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();

        while (std::optional<sf::Event> evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            } else if (auto *kp = evento->getIf<sf::Event::KeyPressed>()) {
                if (estado == JUGANDO) {
                    if (kp->scancode == sf::Keyboard::Scancode::E) {
                        verificarRecoleccion(inv, objetosMundo, MAX_HABITACIONES, jugador);
                        for (int i = 0; i < MAX_SLOTS; i++) {
                            if (inv.espacios[i].tipo == LLAVE) {
                                tieneLlave = true;
                                break;
                            }
                        }
                    } else if (kp->scancode == sf::Keyboard::Scancode::P) {
                        int slot = inv.espacioSeleccionado;
                        if (inv.espacios[slot].tipo != NINGUNO && inv.espacios[slot].placeable) {
                            colocarObjeto(inv, slot, jugador.fila, jugador.columna);
                            inv.espacios[slot].habitacion = habitacionActual;
                        }
                    } else if (kp->scancode == sf::Keyboard::Scancode::Num1) {
                        seleccionarSlot(inv, 0);
                    } else if (kp->scancode == sf::Keyboard::Scancode::Num2) {
                        seleccionarSlot(inv, 1);
                    } else if (kp->scancode == sf::Keyboard::Scancode::Num3) {
                        seleccionarSlot(inv, 2);
                    }
                }
            }
        }

        if (estado == JUGANDO) {
            manejarEntrada(jugador, habitaciones[habitacionActual], dt);
            actualizarEnemigos(enemigos, numEnemigos, jugador,
                               habitaciones[habitacionActual], habitacionActual);

            verificarEfectosObjetos(inv.espacios, MAX_SLOTS,
                                    jugador, enemigos, numEnemigos, habitacionActual);
            actualizarTimersObjetos(inv.espacios, MAX_SLOTS, jugador, dt);

            const Puerta *puerta = obtenerPuertaEn(
                habitaciones[habitacionActual], jugador.fila, jugador.columna);
            bool sobrePuerta = (puerta != nullptr);
            if (sobrePuerta && !sobrePuertaAntes) {
                bool puedePasar = !puerta->bloqueada || tieneLlave;
                if (puedePasar) {
                    int destino = puerta->aHabitacion;
                    habitacionActual = destino;
                    posicionarEnEntrada(habitaciones[habitacionActual],
                                        puerta->aHabitacion, jugador);
                }
            }
            sobrePuertaAntes = sobrePuerta;

            if (verificarVictoria(habitacionActual)) {
                estado = VICTORIA;
            } else if (verificarDerrota(jugador, enemigos, numEnemigos, habitacionActual)) {
                estado = DERROTA;
            }
        }

        ventana.clear(sf::Color::Black);
        dibujarHabitacion(habitaciones[habitacionActual], ventana);
        dibujarObjetosMundo(objetosMundo, MAX_HABITACIONES, ventana, habitacionActual);
        dibujarObjetosSuelo(inv.espacios, MAX_SLOTS, ventana, habitacionActual);
        dibujarEnemigos(enemigos, numEnemigos, ventana, habitacionActual);
        dibujarJugador(jugador, ventana);
        dibujarHUD(ventana, jugador, inv, habitacionActual);
        dibujarOverlayEstado(ventana, estado);
        ventana.display();
    }
}
