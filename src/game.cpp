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

    // Llave en la sala 2 (necesaria para abrir puerta a sala 3)
    Objeto objetosMundo[MAX_HABITACIONES];
    for (int i = 0; i < MAX_HABITACIONES; i++) objetosMundo[i].tipo = NINGUNO;
    inicializarLlaveMundo(objetosMundo[2], 2, 5, 12);

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
                if (estado == JUGANDO && kp->scancode == sf::Keyboard::Scancode::E) {
                    verificarRecoleccion(inv, objetosMundo, MAX_HABITACIONES, jugador);
                    for (int i = 0; i < MAX_SLOTS; i++) {
                        if (inv.espacios[i].tipo == LLAVE) {
                            tieneLlave = true;
                            break;
                        }
                    }
                }
            }
        }

        if (estado == JUGANDO) {
            manejarEntrada(jugador, habitaciones[habitacionActual], dt);
            actualizarEnemigos(enemigos, numEnemigos, jugador,
                               habitaciones[habitacionActual], habitacionActual);

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
        dibujarEnemigos(enemigos, numEnemigos, ventana, habitacionActual);
        dibujarJugador(jugador, ventana);
        dibujarOverlayEstado(ventana, estado);
        ventana.display();
    }
}
