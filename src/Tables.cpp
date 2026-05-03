// ─────────────────────────────────────────────────────────────────────────────
// Tables.cpp  –  Implements all probability / sampling functions.
//
// Naming convention for probabilities
//   p_ann  = annual probability
//   p_mon  = monthly probability  =  1 - (1 - p_ann)^(1/12)
// ─────────────────────────────────────────────────────────────────────────────

#include "Tables.h"
#include "Random.h"

#include <cmath>
#include <vector>

namespace Tables {

// ── Helper: annual → monthly conversion ─────────────────────────────────────
static double toMonthly(double p_annual) {
    return 1.0 - std::pow(1.0 - p_annual, 1.0 / 12.0);
}

// ── 8.1  Desired children ────────────────────────────────────────────────────
int sampleDesiredChildren() {
    // Original weights: {0.6, 0.75, 0.35, 0.20, 0.10, 0.05} – sum = 2.05
    // Using RNG::discrete which normalises internally.
    static const std::vector<double> weights = {0.60, 0.75, 0.35, 0.20, 0.10, 0.05};
    int idx = RNG::get().discrete(weights);
    return idx + 1; // 1, 2, 3, 4, 5, 6  (6 means "> 5")
}

// ── 8.2  Death age ───────────────────────────────────────────────────────────
double generateDeathAge(Sex sex) {
    // Age ranges and conditional probability of dying within each range
    // P(die in range | reached range start)
    static const double bounds[5]  = {0.0, 12.0, 45.0, 76.0, 125.0};
    static const double p_male[4]  = {0.25, 0.10, 0.30, 0.70};
    static const double p_female[4]= {0.25, 0.15, 0.35, 0.65};

    const double* p = (sex == Sex::Male) ? p_male : p_female;

    // Build unconditional probabilities for each range + surviving all
    std::vector<double> unconditional;
    double survival = 1.0;
    for (int i = 0; i < 4; ++i) {
        unconditional.push_back(survival * p[i]);
        survival *= (1.0 - p[i]);
    }
    unconditional.push_back(survival); // survives past 76 without dying → lives to 125

    int range = RNG::get().discrete(unconditional);

    if (range == 4) return 125.0; // maximum lifespan

    double lo = bounds[range];
    double hi = bounds[range + 1];
    return lo + RNG::get().uniformRange(0.0, hi - lo);
}

// ── 8.3  Mourning duration ───────────────────────────────────────────────────
double mourningMeanMonths(double age) {
    if (age < 15.0) return  3.0;   // 3 months
    if (age < 21.0) return  6.0;   // 6 months
    if (age < 35.0) return  6.0;   // 6 months
    if (age < 45.0) return 12.0;   // 1 year
    if (age < 60.0) return 24.0;   // 2 years
    return                 48.0;   // 4 years
}

// ── 8.4  Multiple births ─────────────────────────────────────────────────────
int sampleBirthCount() {
    // Weights sum to 1.02 – RNG::discrete normalises automatically
    static const std::vector<double> weights = {0.70, 0.18, 0.08, 0.04, 0.02};
    return RNG::get().discrete(weights) + 1; // 1 … 5
}

// ── 8.5  Break-up probability ────────────────────────────────────────────────
double breakupProbMonthly() {
    static const double p = toMonthly(0.20);
    return p;
}

// ── 8.6  Pregnancy probability ───────────────────────────────────────────────
double pregnancyProbMonthly(double age) {
    if (age < 12.0) return 0.0;

    double p_annual;
    if      (age < 15.0) p_annual = 0.20;
    else if (age < 21.0) p_annual = 0.45;
    else if (age < 35.0) p_annual = 0.80;
    else if (age < 45.0) p_annual = 0.40;
    else if (age < 60.0) p_annual = 0.20;
    else                 p_annual = 0.05;

    return toMonthly(p_annual);
}

// ── 8.7  Desire for a partner ────────────────────────────────────────────────
double wantPartnerProbMonthly(double age) {
    if (age < 12.0) return 0.0;
    if (age < 15.0) return 0.60;
    if (age < 21.0) return 0.65;
    if (age < 35.0) return 0.80;
    if (age < 45.0) return 0.60;
    if (age < 60.0) return 0.50;
    return                 0.20;
}

// ── 8.8  Couple formation by age difference ──────────────────────────────────
double coupleFormProb(double ageDiffYears) {
    if (ageDiffYears <  5.0) return 0.45;
    if (ageDiffYears < 10.0) return 0.40;
    if (ageDiffYears < 15.0) return 0.35;
    if (ageDiffYears < 20.0) return 0.25;
    return                          0.15;
}

} // namespace Tables