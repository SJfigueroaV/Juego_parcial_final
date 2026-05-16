#include "../include/game.h"
#include "../include/map.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include <SFML/Graphics.hpp>
#include <optional>

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

    sf::RenderWindow ventana(
        sf::VideoMode(sf::Vector2u{MAX_COLUMNAS * TAM_TILE, MAX_FILAS * TAM_TILE}),
        "Dungeon");
    ventana.setFramerateLimit(60);

    sf::Clock reloj;
    bool sobrePuertaAntes = false;

    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();

        while (std::optional<sf::Event> evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        manejarEntrada(jugador, habitaciones[habitacionActual], dt);
        actualizarEnemigos(enemigos, numEnemigos,
                           habitaciones[habitacionActual], habitacionActual);

        const Puerta *puerta = obtenerPuertaEn(
            habitaciones[habitacionActual], jugador.fila, jugador.columna);
        bool sobrePuerta = (puerta != nullptr);
        if (sobrePuerta && !sobrePuertaAntes && !puerta->bloqueada) {
            int destino = puerta->aHabitacion;
            habitacionActual = destino;
            posicionarEnEntrada(habitaciones[habitacionActual],
                                puerta->aHabitacion, jugador);
        }
        sobrePuertaAntes = sobrePuerta;

        ventana.clear(sf::Color::Black);
        dibujarHabitacion(habitaciones[habitacionActual], ventana);
        dibujarEnemigos(enemigos, numEnemigos, ventana, habitacionActual);
        dibujarJugador(jugador, ventana);
        ventana.display();
    }
}
