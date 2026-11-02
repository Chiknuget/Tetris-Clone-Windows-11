#include "game/effects.h"

#include <algorithm>
#include <cmath>

#include "engine/font.h"
#include "ui/theme.h"

namespace {
    constexpr float kGravity = 700.0f;  // pixels per second squared
    constexpr float kTwoPi = 6.2831853f;
}  // namespace

Effects::Effects() : rng_(std::random_device{}()) {}

float Effects::random(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng_);
}

void Effects::clear() {
    particles_.clear();
    popups_.clear();
    shake_time_ = 0.0f;
    shake_x_ = 0;
    shake_y_ = 0;
}

void Effects::burst(float x, float y, Color color, int count, float speed) {
    for (int i = 0; i < count; ++i) {
        const float angle = random(0.0f, kTwoPi);
        const float s = random(0.3f, 1.0f) * speed;
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = std::cos(angle) * s;
        p.vy = std::sin(angle) * s - speed * 0.5f;  // bias upward
        p.max_life = random(0.4f, 0.8f);
        p.life = p.max_life;
        p.size = static_cast<int>(random(2.0f, 5.0f));
        p.color = color;
        particles_.push_back(p);
    }
}

void Effects::popup(const std::string& text, float x, float y, Color color, int scale) {
    Popup p;
    p.text = text;
    p.x = x;
    p.y = y;
    p.max_life = 1.0f;
    p.life = p.max_life;
    p.color = color;
    p.scale = scale;
    popups_.push_back(p);
}

void Effects::shake(float strength, float duration) {
    shake_strength_ = std::max(shake_strength_ * (shake_time_ > 0.0f ? 1.0f : 0.0f), strength);
    shake_duration_ = duration;
    shake_time_ = duration;
}

void Effects::update(double dt) {
    const float t = static_cast<float>(dt);

    for (Particle& p : particles_) {
        p.vy += kGravity * t;
        p.x += p.vx * t;
        p.y += p.vy * t;
        p.life -= t;
    }
    particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
        [](const Particle& p) { return p.life <= 0.0f; }),
        particles_.end());

    for (Popup& p : popups_) {
        p.y -= 40.0f * t;
        p.life -= t;
    }
    popups_.erase(std::remove_if(popups_.begin(), popups_.end(),
        [](const Popup& p) { return p.life <= 0.0f; }),
        popups_.end());

    if (shake_time_ > 0.0f) {
        shake_time_ -= t;
        const float s = shake_strength_ * std::max(0.0f, shake_time_ / shake_duration_);
        shake_x_ = static_cast<int>(random(-s, s));
        shake_y_ = static_cast<int>(random(-s, s));
    }
    else {
        shake_x_ = 0;
        shake_y_ = 0;
    }
}

void Effects::draw(Canvas& canvas) const {
    for (const Particle& p : particles_) {
        const float alpha = p.life / p.max_life;
        canvas.blend_rect(static_cast<int>(p.x) - p.size / 2, static_cast<int>(p.y) - p.size / 2,
            p.size, p.size, p.color, alpha);
    }
    for (const Popup& p : popups_) {
        const float fade = std::min(1.0f, (p.life / p.max_life) * 2.0f);
        const Color c = mix(theme::kBoardBg, p.color, fade);
        const int w = font::text_width(p.text, p.scale);
        font::draw_text(canvas, static_cast<int>(p.x) - w / 2, static_cast<int>(p.y), p.text, c, p.scale);
    }
}
