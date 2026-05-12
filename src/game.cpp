#include "../include/game.h"
#include "../include/map.h"
#include <SFML/Graphics.hpp>
#include <optional>

void ejecutarJuego() {
    Habitacion habitacion;
    inicializarHabitacionVacia(habitacion);

    sf::RenderWindow ventana(
        sf::VideoMode(sf::Vector2u{MAX_COLUMNAS * TAM_TILE, MAX_FILAS * TAM_TILE}),
        "Dungeon");
    ventana.setFramerateLimit(60);

    while (ventana.isOpen()) {
        while (std::optional<sf::Event> evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }
        ventana.clear(sf::Color::Black);
        dibujarHabitacion(habitacion, ventana);
        ventana.display();
    }
}
