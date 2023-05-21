#include <SFML/Graphics.hpp>

#include "solver.hpp"

// window settings
const int FPS = 60;
const int WIDTH = 1200, HEIGHT = 800;
const int PPM = 100;  // pixels per meter

// physics settings
const float ACC = 9.8 * PPM;  // acceleration due to gravity

int main() {
    auto window = sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Verlet Integration");
    window.setFramerateLimit(FPS);

    auto solver = ObjectSolver();
    solver.bounds = CircleBounds(sf::Vector2f(WIDTH / 2.0, HEIGHT / 2.0), 400, sf::Color(180, 180, 180));

    const float dt = 1.0 / FPS;
    while (window.isOpen()) {
        auto event = sf::Event();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                auto pos = sf::Mouse::getPosition(window);
                float radius = rand() % 15 + 5;
                sf::Color color(rand() % 255, rand() % 255, rand() % 255);
                auto circle = CircleObject(sf::Vector2f(pos.x, pos.y), sf::Vector2f(0, ACC), radius, color);
                solver.add_circle(circle);
            }
        }

        window.clear();

        solver.update(dt);
        solver.draw(window);

        window.display();
    }
}