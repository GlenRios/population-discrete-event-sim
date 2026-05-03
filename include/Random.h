#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Random.h  –  Singleton wrapper around a 64-bit Mersenne Twister
// ─────────────────────────────────────────────────────────────────────────────

#include <random>
#include <vector>

class RNG {
public:
    /// Global singleton access
    static RNG& get();

    /// Reseed the engine (call before Simulation::run() for reproducibility)
    void seed(unsigned s);

    /// Uniform real in [0, 1)
    double uniform();

    /// Uniform real in [lo, hi)
    double uniformRange(double lo, double hi);

    /// Exponential variate with given mean (in whatever unit you use)
    double exponential(double mean);

    /// Sample index from a (possibly un-normalised) discrete distribution
    /// Returns the index of the chosen bucket.
    int discrete(const std::vector<double>& weights);

    /// Expose engine so std::shuffle can use the same source
    std::mt19937_64& engine() { return engine_; }

private:
    RNG();
    std::mt19937_64 engine_;
};