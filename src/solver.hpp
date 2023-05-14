#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

struct CircleObject {
    sf::Vector2f pos, last_pos, acc;
    float radius;
    sf::Color color;

    CircleObject() = default;
    CircleObject(sf::Vector2f pos, sf::Vector2f acc, float radius, sf::Color color) : pos(pos), last_pos(pos), acc(acc), radius(radius), color(color) {}

    sf::Vector2f get_center() const {
        return pos - sf::Vector2f(radius, radius);
    }
    void set_velocity(sf::Vector2f vel) {
        last_pos = pos - vel;
    }
    void update(float dt) {
        auto new_pos = pos * 2.0f - last_pos + acc * dt * dt;
        last_pos = pos;
        pos = new_pos;
    }
    void draw(sf::RenderWindow& window) {
        auto circle = sf::CircleShape(radius);
        circle.setPosition(get_center());
        circle.setFillColor(color);
        window.draw(circle);
    }
};

struct CircleBounds : CircleObject {
    CircleBounds() = default;
    CircleBounds(sf::Vector2f pos, float radius, sf::Color color) : CircleObject(pos, sf::Vector2f(0, 0), radius, color) {}

    void update(float dt) {
        // do nothing
    }
    void restrict_object(CircleObject& object) {
        auto [dx, dy] = object.pos - pos;
        auto dist = std::hypot(dx, dy);
        if (dist + object.radius > radius) {
            // idk how any of this works
            auto [nx, ny] = sf::Vector2f(dx, dy) / dist;
            auto [vx, vy] = object.pos - object.last_pos;
            auto [tx, ty] = sf::Vector2f(-ny, nx);
            auto dp = vx * tx + vy * ty;
            object.pos = pos + sf::Vector2f(nx, ny) * (radius - object.radius);
            object.last_pos = object.pos - sf::Vector2f(tx, ty) * dp;
        }
    }
};

struct ObjectSolver {
    std::vector<CircleObject> circles;
    CircleBounds bounds;

    ObjectSolver() = default;
    ObjectSolver(sf::Vector2f bounds_pos, float bounds_radius, sf::Color bounds_color) : bounds(bounds_pos, bounds_radius, bounds_color) {}

    void add_circle(CircleObject circle) {
        circles.push_back(circle);
    }
    void add_circle(sf::Vector2f pos, sf::Vector2f acc, float radius, sf::Color color) {
        circles.emplace_back(pos, acc, radius, color);
    }
    void handle_collisions() {
        for (size_t i = 0; i < circles.size(); i++) {
            for (size_t j = 0; j < circles.size(); j++) {
                if (i == j) {
                    continue;
                }

                auto &circle1 = circles[i], &circle2 = circles[j];
                auto [dx, dy] = circle1.pos - circle2.pos;
                auto dist = std::hypot(dx, dy);
                if (dist > circle1.radius + circle2.radius) {
                    continue;
                }
                auto shift = (circle1.radius + circle2.radius - dist) / 2.0f;
                auto n = sf::Vector2f(dx, dy) / dist * shift;
                circle1.pos += n;
                circle2.pos -= n;
            }
        }
    }
    void update(float dt) {
        for (auto& circle : circles) {
            circle.update(dt);
        }
        for (int i = 0; i < 8; i++) {
            handle_collisions();
            for (auto& circle : circles) {
                bounds.restrict_object(circle);
            }
        }
    }
    void draw(sf::RenderWindow& window) {
        bounds.draw(window);
        for (auto& circle : circles) {
            circle.draw(window);
        }
    }
};