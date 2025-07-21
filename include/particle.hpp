#pragma once
#include "vec2d.hpp"

class Particle {
public:
    size_t id;
    Vec2d position;
    Vec2d velocity;
    Vec2d acceleration;
    double mass;

private:
    static size_t next_id; // Статический счётчик для генерации уникальных id

public:
    // Конструкторы
    Particle(const Vec2d& _pos)
      : id(next_id++),
        position(_pos),
        velocity(0, 0),
        acceleration(0, 0),
        mass(1) {}

    Particle(const Vec2d& _pos, double _mass)
      : id(next_id++),
        position(_pos),
        velocity(0, 0),
        acceleration(0, 0),
        mass(_mass) {}

    Particle(const Vec2d& _pos, const Vec2d& _vel, double _mass)
      : id(next_id++),
        position(_pos),
        velocity(_vel),
        acceleration(0, 0),
        mass(_mass) {}

    bool operator<(const Particle& other) const {
        return this->id < other.id;
    }
};

// Инициализация статического счётчика
size_t Particle::next_id = 0;