#pragma once

#pragma once

#include <random>
#include <string>
#include <vector>

#include "engine/canvas.h"

// Particles, floating text and screen shake. Positions are in canvas pixels.
class Effects {
public:
    Effects();

    void clear();
    void update(double dt);
    void draw(Canvas& canvas) const;

    void burst(float x, float y, Color color, int count, float speed);
    void popup(const std::string& text, float x, float y, Color color, int scale);
    void shake(float strength, float duration);

    int shake_x() const { return shake_x_; }
    int shake_y() const { return shake_y_; }

private:
    struct Particle {
        float x, y, vx, vy, life, max_life;
        int size;
        Color color;
    };
    struct Popup {
        std::string text;
        float x, y, life, max_life;
        Color color;
        int scale;
    };

    float random(float lo, float hi);

    std::vector<Particle> particles_;
    std::vector<Popup> popups_;
    std::mt19937 rng_;

    float shake_time_ = 0.0f;
    float shake_duration_ = 0.0f;
    float shake_strength_ = 0.0f;
    int shake_x_ = 0;
    int shake_y_ = 0;
};
