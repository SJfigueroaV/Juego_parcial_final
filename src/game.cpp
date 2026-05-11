#include "../include/game.h"
#include <SFML/Graphics.hpp>
#include <optional>

void ejecutarJuego() {
    sf::RenderWindow ventana(sf::VideoMode(sf::Vector2u{800, 600}), "Dungeon");
    ventana.setFramerateLimit(60);

    while (ventana.isOpen()) {
        while (std::optional<sf::Event> evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }
        ventana.clear(sf::Color::Black);
        ventana.display();
    }
}
