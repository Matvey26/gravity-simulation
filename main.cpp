#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include "include/utils.hpp"
#include "include/particle.hpp"
#include "include/vec2d.hpp"
#include "include/tree.hpp"

// Параметры экрана
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
// Временной шаг
const double DT = 0.0001;

struct Points {
    std::vector<Particle> particles;
    sf::VertexArray points;

    Points(const std::vector<Particle>& _particles)
    : particles(_particles)
    , points(sf::Points, _particles.size()) {
        for (size_t i = 0; i < particles.size(); ++i) {
            points[i].position = sf::Vector2f(
                particles[i].position.x,
                particles[i].position.y
            );
            points[i].color = sf::Color(0, 120, 255);
        }
    }

    size_t size() const {
        return particles.size();
    }

    void update() {
        for (size_t i = 0; i < particles.size(); ++i) {
            points[i].position.x = particles[i].position.x;
            points[i].position.y = particles[i].position.y;
        }
    }
};

int main() {
    // Создаем окно SFML
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Gravitational Simulation");
    window.setFramerateLimit(30);
    
    // Создаем генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Создаем частицы
    std::vector<Particle> particles;
    {
        double tmp_radius_spawn = 400;
        double tmp_cx = SCREEN_WIDTH / 2;
        double tmp_cy = SCREEN_HEIGHT / 2;
        for (double i = 0; i < SCREEN_WIDTH; i += 2) {
            for (double j = 0; j < SCREEN_HEIGHT; j += 2) {
                double dist = square(tmp_cx - i) + square(tmp_cy - j);
                if (dist <= square(tmp_radius_spawn) and dist > 100) {
                    dist = std::sqrt(dist);
                    double probability = 1 - square(dist / tmp_radius_spawn);
                    if (dis(gen) < probability) {
                        double pseudo_massa = 1e5 * square(dist);
                        double velocity = std::sqrt(pseudo_massa / dist);
                        Vec2d velocity_vec = velocity * Vec2d(j - tmp_cy, tmp_cx - i).normalize();
                        particles.push_back({{i, j}, velocity_vec, 1e6});
                    }
                }
                // double probability = 0.2;
                // if (dis(gen) < probability) {
                //     particles.push_back({{i, j}, {dis(gen), dis(gen)}, 1e6});
                // }
            }
        }
        std::cout << "Number of particles: " << particles.size() << std::endl;
    }
    Points points(particles);

    // Корневой узел квадродерева
    QuadTreeNode root;
    root.corner = {0, 0};
    root.width = 2000;
    root.height = 2000;

    // Переменные для подсчета FPS
    sf::Clock clock;
    std::vector<float> frameTimes;
    const size_t FRAME_AVERAGE_COUNT = 10; // Количество кадров для усреднения FPS

    // Главный цикл
    int i = 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Обновляем симуляцию
        simulate_step(points.particles, root, DT);
        points.update();

        // Очищаем экран
        window.clear();

        // Рисуем частицы
        window.draw(points.points);

        // Отображаем всё на экране
        window.display();

        // Подсчет FPS
        float frameTime = clock.restart().asSeconds();
        frameTimes.push_back(frameTime);

        // Удаляем старые времена кадров, если их больше FRAME_AVERAGE_COUNT
        if (frameTimes.size() > FRAME_AVERAGE_COUNT) {
            frameTimes.erase(frameTimes.begin());
        }

        // Вычисляем среднее время кадра и FPS
        if (frameTimes.size() == FRAME_AVERAGE_COUNT) {
            float averageFrameTime = 0.0f;
            for (float time : frameTimes) {
                averageFrameTime += time;
            }
            averageFrameTime /= FRAME_AVERAGE_COUNT;
            float fps = 1.0f / averageFrameTime;

            // Выводим FPS в терминал
            std::cout << "; FPS: " << fps;
        }
    }

    return 0;
}