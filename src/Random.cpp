// ─────────────────────────────────────────────────────────────────────────────
// Random.cpp
// ─────────────────────────────────────────────────────────────────────────────

#include "Random.h"
#include <cmath>
#include <numeric>
#include <stdexcept>

// ── Singleton ────────────────────────────────────────────────────────────────
RNG& RNG::get() {
    static RNG instance;
    return instance;
}

RNG::RNG() : engine_(std::random_device{}()) {}

// ── Public API ───────────────────────────────────────────────────────────────
void RNG::seed(unsigned s) {
    engine_.seed(s);
}

double RNG::uniform() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine_);
}

double RNG::uniformRange(double lo, double hi) {
    std::uniform_real_distribution<double> dist(lo, hi);
    return dist(engine_);
}

double RNG::exponential(double mean) {
    if (mean <= 0.0) return 0.0;
    // Generate via inverse CDF: -mean * ln(U)
    double u = uniform();
    // Guard against u == 0 to avoid log(0)
    if (u <= 0.0) u = 1e-15;
    return -mean * std::log(u);
}

int RNG::discrete(const std::vector<double>& weights) {
    if (weights.empty()) return 0;

    double total = std::accumulate(weights.begin(), weights.end(), 0.0);
    double u     = uniform() * total;
    double cumul = 0.0;

    for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
        cumul += weights[i];
        if (u < cumul) return i;
    }
    return static_cast<int>(weights.size()) - 1; // rounding safety
}