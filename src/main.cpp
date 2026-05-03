// ─────────────────────────────────────────────────────────────────────────────
// main.cpp  –  Entry point for the population simulation
//
// Usage:
//   ./population_sim [males] [females] [years] [seed]
//
// Defaults: 500 males, 500 females, 100 years, seed = 42
// ─────────────────────────────────────────────────────────────────────────────

#include "Simulation.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // ── Parse command-line arguments (all optional) ───────────────────────────
    int  num_males   = 500;
    int  num_females = 500;
    int  years       = 100;
    unsigned seed    = 42;

    auto toInt  = [](const char* s) { return std::stoi(s); };
    auto toUInt = [](const char* s) { return static_cast<unsigned>(std::stoul(s)); };

    if (argc > 1) num_males   = toInt(argv[1]);
    if (argc > 2) num_females = toInt(argv[2]);
    if (argc > 3) years       = toInt(argv[3]);
    if (argc > 4) seed        = toUInt(argv[4]);

    // ── Banner ────────────────────────────────────────────────────────────────
    std::cout << "╔══════════════════════════════════════════════════╗\n"
              << "║       Discrete-Event Population Simulation       ║\n"
              << "╚══════════════════════════════════════════════════╝\n\n"
              << "  Initial males   : " << num_males   << "\n"
              << "  Initial females : " << num_females << "\n"
              << "  Simulation years: " << years       << "\n"
              << "  RNG seed        : " << seed        << "\n\n";

    // ── Run ───────────────────────────────────────────────────────────────────
    Simulation sim(num_males, num_females, seed);
    sim.run(years);

    // ── Output ────────────────────────────────────────────────────────────────
    sim.printSummary();

    const std::string csv_file = "results.csv";
    sim.saveResults(csv_file);

    return 0;
}