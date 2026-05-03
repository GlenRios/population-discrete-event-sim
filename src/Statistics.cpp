// ─────────────────────────────────────────────────────────────────────────────
// Statistics.cpp
// ─────────────────────────────────────────────────────────────────────────────

#include "Statistics.h"

#include <fstream>
#include <iomanip>
#include <iostream>

void Statistics::record(const YearSnapshot& snap) {
    records_.push_back(snap);
}

void Statistics::printTable() const {
    constexpr int W = 10;

    // Header
    std::cout << std::right
              << std::setw(5)  << "Year"
              << std::setw(W)  << "Pop"
              << std::setw(W)  << "Males"
              << std::setw(W)  << "Females"
              << std::setw(W)  << "Births"
              << std::setw(W)  << "Deaths"
              << std::setw(W)  << "NewPairs"
              << std::setw(W)  << "Breakups"
              << std::setw(W)  << "Pairs"
              << std::setw(12) << "AvgKids/W"
              << "\n";
    std::cout << std::string(5 + W * 8 + 12, '-') << "\n";

    for (const auto& s : records_) {
        std::cout << std::setw(5)  << s.year
                  << std::setw(W)  << s.total_pop
                  << std::setw(W)  << s.males
                  << std::setw(W)  << s.females
                  << std::setw(W)  << s.births
                  << std::setw(W)  << s.deaths
                  << std::setw(W)  << s.couples_formed
                  << std::setw(W)  << s.breakups
                  << std::setw(W)  << s.active_couples
                  << std::setw(12) << std::fixed << std::setprecision(2)
                                   << s.avg_children_per_woman
                  << "\n";
    }
}

void Statistics::saveCSV(const std::string& filename) const {
    std::ofstream f(filename);
    if (!f) {
        std::cerr << "[Statistics] Cannot open file for writing: " << filename << "\n";
        return;
    }

    f << "year,total_pop,males,females,births,deaths,"
         "couples_formed,breakups,active_couples,avg_children_per_woman\n";

    for (const auto& s : records_) {
        f << s.year           << ","
          << s.total_pop      << ","
          << s.males          << ","
          << s.females        << ","
          << s.births         << ","
          << s.deaths         << ","
          << s.couples_formed << ","
          << s.breakups       << ","
          << s.active_couples << ","
          << std::fixed << std::setprecision(4)
          << s.avg_children_per_woman << "\n";
    }

    std::cout << "[Statistics] Results saved to '" << filename << "'\n";
}