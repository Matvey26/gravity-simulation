#pragma once
#include <algorithm>
#include <cmath>

class Vec2d {
public:  // Добавлен модификатор доступа public
    double x = 0;
    double y = 0;
    Vec2d() = default;
    Vec2d(double _x, double _y) : x(_x), y(_y) {}
    // Vec2d(size_t _x, size_t _y) : x(_x), y(_y) {}
    Vec2d(const Vec2d& other) : x(other.x), y(other.y) {}
    
    Vec2d& operator=(const Vec2d& other) {
        this->x = other.x;
        this->y = other.y;
        return *this;
    }

    // Арифметические операции
    Vec2d operator+(const Vec2d& other) const {
        return Vec2d(x + other.x, y + other.y);
    }
    
    Vec2d operator-(const Vec2d& other) const {
        return Vec2d(x - other.x, y - other.y);
    }
    
    Vec2d operator*(double scalar) const {
        return Vec2d(x * scalar, y * scalar);
    }
    
    Vec2d operator/(double scalar) const {
        return Vec2d(x / scalar, y / scalar);
    }
    
    Vec2d& operator+=(const Vec2d& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vec2d& operator-=(const Vec2d& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vec2d& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    Vec2d& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    void swap(Vec2d& other) {
        std::swap(other.x, this->x);
        std::swap(other.y, this->y);
    }

    double dot(const Vec2d& other) const {
        return this->x * other.x + this->y * other.y;
    }

    double square_length() const {
        return this->dot(*this);
    }

    double length() const {
        return std::sqrt(this->square_length());
    }

    Vec2d normalize() const {
        double len = this->length();
        if (len < 1e-6) {
            return *this;
        }
        return (*this) / this->length();
    }
};

inline Vec2d operator*(double scalar, const Vec2d& vec) {
    return Vec2d(vec.x * scalar, vec.y * scalar);
}

namespace std {
    template<>
    void swap(Vec2d& a, Vec2d& b) {
        a.swap(b);
    }
}


// Функция для обрезки вектора до максимальной длины
Vec2d clamp_length(const Vec2d& vec, double max_length) {
    double current_length = vec.length();
    if (current_length > max_length) {
        return vec.normalize() * max_length;
    }
    return vec;
}
