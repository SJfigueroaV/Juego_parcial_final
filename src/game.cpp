#include "../include/game.h"
#include "../include/map.h"
#include "../include/player.h"
#include <SFML/Graphics.hpp>
#include <optional>

void ejecutarJuego() {
    Habitacion habitaciones[MAX_HABITACIONES];
    int numHabitaciones = 0;
    inicializarHabitaciones(habitaciones, numHabitaciones);

    int habitacionActual = 0;

    Jugador jugador;
    inicializarJugador(jugador, 10, 13);

    sf::RenderWindow ventana(
        sf::VideoMode(sf::Vector2u{MAX_COLUMNAS * TAM_TILE, MAX_FILAS * TAM_TILE}),
        "Dungeon");
    ventana.setFramerateLimit(60);

    sf::Clock reloj;

    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();

        while (std::optional<sf::Event> evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        manejarEntrada(jugador, habitaciones[habitacionActual], dt);

        ventana.clear(sf::Color::Black);
        dibujarHabitacion(habitaciones[habitacionActual], ventana);
        dibujarJugador(jugador, ventana);
        ventana.display();
    }
}
