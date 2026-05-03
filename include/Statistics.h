#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Statistics.h  –  Per-year snapshot and final reporting
// ─────────────────────────────────────────────────────────────────────────────

#include <string>
#include <vector>

struct YearSnapshot {
    int    year                  = 0;
    int    total_pop             = 0;
    int    males                 = 0;
    int    females               = 0;
    int    births                = 0;
    int    deaths                = 0;
    int    couples_formed        = 0;
    int    breakups              = 0;
    int    active_couples        = 0;  // couples alive at year-end
    double avg_children_per_woman = 0.0;
};

class Statistics {
public:
    void record(const YearSnapshot& snap);

    /// Pretty-print a table to stdout
    void printTable() const;

    /// Write all snapshots as CSV
    void saveCSV(const std::string& filename) const;

    /// Read-only access to the underlying data
    const std::vector<YearSnapshot>& data() const { return records_; }

private:
    std::vector<YearSnapshot> records_;
};