#pragma once
#include "vec2d.hpp"
#include "particle.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <stack>
#include <limits>
#include <iostream>
#include <iomanip>


struct QuadTreeNode {
    Vec2d corner = {0, 0};
    double width = 0, height = 0;
    double total_mass = 0;
    Vec2d center_mass = {0, 0};
    std::vector<Particle> particles;
    QuadTreeNode* children[4] = {nullptr, nullptr, nullptr, nullptr};

    QuadTreeNode() = default;

    QuadTreeNode(const QuadTreeNode& other)
      : corner(other.corner),
        width(other.width),
        height(other.height),
        total_mass(0),
        center_mass({0, 0}) {}
    
    ~QuadTreeNode() {
        for (int i = 0; i < 4; ++i) {
            delete children[i];
        }
    }
    
    bool is_leaf() const {
        return !children[0] && !children[1] && !children[2] && !children[3];
    }

    bool has(const Particle& particle) const {
        return particle.position.x >= this->corner.x &&
               particle.position.x <= this->corner.x + this->width &&
               particle.position.y >= this->corner.y &&
               particle.position.y <= this->corner.y + this->height;
    }
};

const int MAX_PARTICLES_PER_NODE = 500;
const double MIN_WIDTH = 2;
const double MIN_HEIGHT = 2;
const double theta = 1;  // Чем ближе к 0, тем точнее расчёты, чем больше, тем грубее расчёты

void insert_particle(QuadTreeNode& node, const Particle& particle, bool is_splitting = false) {
    if (!node.is_leaf() or is_splitting) {
        // Определяем, в какой квадрант попадает частица
        double midX = node.corner.x + node.width / 2;
        double midY = node.corner.y + node.height / 2;

        int index = 0;
        if (particle.position.x >= midX) index += 1;
        if (particle.position.y >= midY) index += 2;

        if (!node.children[index]) {
            // Создаем новый дочерний узел, если его нет
            double child_width = node.width / 2;
            double child_height = node.height / 2;
            node.children[index] = new QuadTreeNode();
            node.children[index]->corner.x = node.corner.x + (index % 2) * child_width;
            node.children[index]->corner.y = node.corner.y + (index / 2) * child_height;
            node.children[index]->width = child_width;
            node.children[index]->height = child_height;
        }

        // Вставляем частицу в соответствующий дочерний узел
        insert_particle(*node.children[index], particle, false);
    } else {
        // Если узел является листом, добавляем частицу
        node.particles.push_back(particle);

        if (node.particles.size() > MAX_PARTICLES_PER_NODE and
            node.width > MIN_WIDTH and node.height > MIN_HEIGHT)
        {
            for (const auto& p : node.particles) {
                insert_particle(node, p, true); // Перераспределяем частицы
            }
            node.particles.clear();
        }
    }

    // Обновляем массу и центр масс
    if (!is_splitting) {
        node.total_mass += particle.mass;
        node.center_mass = (node.center_mass * (node.total_mass - particle.mass)
                            + particle.position * particle.mass) / node.total_mass;
    }
}

Vec2d calculate_acceleration(const Particle& particle, const QuadTreeNode& node, size_t& count_operations) {
    ++count_operations;
    Vec2d r = node.center_mass - particle.position;
    double distance_square = r.square_length();

    if (distance_square == 0) return {0, 0};

    double distance = r.length();
    double size = std::max(node.width, node.height);

    if (node.is_leaf() and node.has(particle)) {
        return {0, 0};
    }
    if (size / distance < theta || node.is_leaf()) {
        // Аппроксимируем узел как одну частицу
        double acceleration = node.total_mass / distance_square;
        return r.normalize() * acceleration;
    } else {
        // Рекурсивно обходим дочерние узлы
        Vec2d total_acceleration{0, 0};
        for (int i = 0; i < 4; ++i) {
            if (node.children[i]) {
                total_acceleration += calculate_acceleration(particle, *node.children[i], count_operations);
            }
        }
        return total_acceleration;
    }
}

double current_average = 0.0;
void simulate_step(std::vector<Particle>& particles, QuadTreeNode& root, double dt) {
    // Строим новое квадродерево
    root = QuadTreeNode(root);
    for (const auto& particle : particles) {
        insert_particle(root, particle);
    }

    // Обновляем положения
    size_t cnt = 0;
    for (auto& particle : particles) {
        particle.position += particle.velocity * dt + particle.acceleration * dt * dt * 0.5;

        Vec2d new_acceleration = calculate_acceleration(particle, root, cnt);
        particle.velocity += (particle.acceleration + new_acceleration) * 0.5 * dt;
        particle.acceleration = new_acceleration;

        // Клиппинг положения. Чтобы частицы не улетали за пределы экрана
        particle.position.x = std::max(std::min(particle.position.x, root.width + root.corner.x), root.corner.x);
        particle.position.y = std::max(std::min(particle.position.y, root.height + root.corner.y), root.corner.y);
    }
    // std::cout << cnt << std::endl;
    current_average = 0.5 * current_average + 0.5 * cnt;
    std::cout << "\rNumber of operations per frame: " << std::fixed << std::setprecision(2) << current_average << std::flush;
}


