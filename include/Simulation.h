#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Simulation.h  –  Core discrete-event engine (monthly time steps)
// ─────────────────────────────────────────────────────────────────────────────

#include "Person.h"
#include "Statistics.h"

#include <string>
#include <unordered_map>

class Simulation {
public:
    /// @param num_males   Initial number of male individuals
    /// @param num_females Initial number of female individuals
    /// @param seed        RNG seed (same seed → reproducible run)
    Simulation(int num_males, int num_females, unsigned seed = 42);

    /// Run the simulation for the requested number of years (default 100).
    void run(int years = 100);

    /// Print the yearly statistics table to stdout.
    void printSummary() const;

    /// Save yearly statistics to a CSV file.
    void saveResults(const std::string& filename) const;

    // ── Inspection ───────────────────────────────────────────────────────────
    int  populationSize()   const { return static_cast<int>(people_.size()); }
    const Statistics& stats() const { return stats_; }

private:
    // ── Initialization ───────────────────────────────────────────────────────
    void initialize();
    Person createPerson(Sex sex, double age = 0.0);

    // ── Monthly phases (executed in this order each step) ────────────────────
    void phaseUpdateAges();
    void phaseDeaths();
    void phaseBirths();
    void phaseMourning();
    void phaseBreakups();
    void phasePregnancies();
    void phaseCoupleFormation();

    // ── Helpers ──────────────────────────────────────────────────────────────
    /// Break up couple (id1, id2).  If death=true only id2's state is updated.
    void dissolveCouple(int id1, int id2, bool deathOfId1 = false);
    void enterMourning(Person& p);

    /// Snapshot yearly statistics
    void recordYear(int year);

    // ── State ────────────────────────────────────────────────────────────────
    int  next_id_        = 0;
    int  init_males_     = 0;
    int  init_females_   = 0;

    std::unordered_map<int, Person> people_;   // id → Person

    // Per-year accumulators (reset every 12 months)
    int yr_births_          = 0;
    int yr_deaths_          = 0;
    int yr_couples_formed_  = 0;
    int yr_breakups_        = 0;

    Statistics stats_;
};