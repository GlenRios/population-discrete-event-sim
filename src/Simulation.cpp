// ─────────────────────────────────────────────────────────────────────────────
// Simulation.cpp  –  Discrete-event population simulation (monthly steps)
//
// Each month the following phases run in order:
//   1. Update ages
//   2. Deaths
//   3. Births
//   4. End of mourning
//   5. Couple break-ups
//   6. New pregnancies
//   7. New couple formation
// ─────────────────────────────────────────────────────────────────────────────

#include "Simulation.h"
#include "Tables.h"
#include "Random.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <vector>

// ── Constructor ───────────────────────────────────────────────────────────────
Simulation::Simulation(int num_males, int num_females, unsigned seed)
    : init_males_(num_males), init_females_(num_females)
{
    RNG::get().seed(seed);
}

// ── Public: run ───────────────────────────────────────────────────────────────
void Simulation::run(int years) {
    initialize();

    const int total_months = years * 12;
    int current_year = 0;

    for (int month = 1; month <= total_months; ++month) {
        phaseUpdateAges();
        phaseDeaths();
        phaseBirths();
        phaseMourning();
        phaseBreakups();
        phasePregnancies();
        phaseCoupleFormation();

        if (month % 12 == 0) {
            ++current_year;
            recordYear(current_year);
            // Reset yearly accumulators
            yr_births_         = 0;
            yr_deaths_         = 0;
            yr_couples_formed_ = 0;
            yr_breakups_       = 0;
        }
    }
}

// ── Public: output ────────────────────────────────────────────────────────────
void Simulation::printSummary() const {
    std::cout << "\n=== Population Simulation – Yearly Summary ===\n\n";
    stats_.printTable();
    std::cout << "\nFinal population: " << people_.size() << " individuals\n";
}

void Simulation::saveResults(const std::string& filename) const {
    stats_.saveCSV(filename);
}

// ═════════════════════════════════════════════════════════════════════════════
// Initialization
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::initialize() {
    next_id_  = 0;
    people_.clear();
    yr_births_ = yr_deaths_ = yr_couples_formed_ = yr_breakups_ = 0;

    // Initial population: ages drawn uniformly from [0, 100)
    for (int i = 0; i < init_males_;   ++i) {
        Person p = createPerson(Sex::Male,   RNG::get().uniformRange(0.0, 100.0));
        people_[p.id] = p;
    }
    for (int i = 0; i < init_females_; ++i) {
        Person p = createPerson(Sex::Female, RNG::get().uniformRange(0.0, 100.0));
        people_[p.id] = p;
    }
}

Person Simulation::createPerson(Sex sex, double age) {
    Person p;
    p.id               = next_id_++;
    p.sex              = sex;
    p.age              = age;
    p.death_age        = Tables::generateDeathAge(sex);
    p.desired_children = Tables::sampleDesiredChildren();
    p.children_had     = 0;
    p.status           = Status::Single;
    p.partner_id       = -1;
    p.mourning_months  = 0.0;
    p.pregnancy        = 0.0;
    return p;
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 1 – Update ages
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phaseUpdateAges() {
    for (auto& [id, p] : people_) {
        p.age += 1.0 / 12.0;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 2 – Deaths
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phaseDeaths() {
    // Collect the IDs of those who die this month (avoid modifying while iterating)
    std::vector<int> dying;
    for (const auto& [id, p] : people_) {
        if (p.age >= p.death_age || p.age >= 125.0) {
            dying.push_back(id);
        }
    }

    for (int id : dying) {
        auto it = people_.find(id);
        if (it == people_.end()) continue; // already gone (shouldn't happen)

        const Person& p = it->second;
        ++yr_deaths_;

        // If this person had a partner, the partner enters mourning
        if (p.isCoupled() && p.partner_id != -1) {
            auto pit = people_.find(p.partner_id);
            if (pit != people_.end()) {
                enterMourning(pit->second);
            }
        }
        // If a pregnant woman dies, pregnancy is lost (no action needed –
        // the Person is simply removed from the map).

        people_.erase(it);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 3 – Births
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phaseBirths() {
    std::vector<Person> newborns;

    for (auto& [id, p] : people_) {
        if (!p.isFemale() || !p.isPregnant()) continue;

        p.pregnancy += 1.0; // advance gestational age by one month

        // Full-term: 9 months of gestation (pregnancy starts at 1, so >= 9)
        if (p.pregnancy >= 9.0) {
            int count = Tables::sampleBirthCount();

            for (int i = 0; i < count; ++i) {
                Sex baby_sex = (RNG::get().uniform() < 0.5) ? Sex::Male : Sex::Female;
                Person baby  = createPerson(baby_sex, 0.0);
                newborns.push_back(baby);
                ++yr_births_;

                // Update parent child counters
                p.children_had++;
                if (p.partner_id != -1) {
                    auto pit = people_.find(p.partner_id);
                    if (pit != people_.end()) {
                        pit->second.children_had++;
                    }
                }
            }

            p.pregnancy = 0.0; // pregnancy ends
        }
    }

    // Add newborns to the population
    for (auto& baby : newborns) {
        people_[baby.id] = baby;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 4 – End of mourning
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phaseMourning() {
    for (auto& [id, p] : people_) {
        if (!p.isMourning()) continue;
        p.mourning_months -= 1.0;
        if (p.mourning_months <= 0.0) {
            p.status        = Status::Single;
            p.mourning_months = 0.0;
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 5 – Couple break-ups
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phaseBreakups() {
    const double p_break = Tables::breakupProbMonthly();

    // Enumerate unique couples: record (id of lower, id of higher) once
    std::unordered_set<int> seen;
    std::vector<std::pair<int,int>> couples;

    for (const auto& [id, p] : people_) {
        if (!p.isCoupled() || p.partner_id == -1) continue;
        if (seen.count(id)) continue;

        seen.insert(id);
        seen.insert(p.partner_id);
        couples.emplace_back(id, p.partner_id);
    }

    for (const auto& [id1, id2] : couples) {
        if (RNG::get().uniform() >= p_break) continue; // couple survives

        ++yr_breakups_;
        dissolveCouple(id1, id2);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 6 – New pregnancies
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phasePregnancies() {
    for (auto& [id, p] : people_) {
        if (!p.isFemale())          continue;
        if (!p.isCoupled())         continue;
        if (p.isPregnant())         continue;
        if (!p.wantsMoreChildren()) continue;
        if (p.age < 12.0)           continue;

        double prob = Tables::pregnancyProbMonthly(p.age);
        if (RNG::get().uniform() < prob) {
            p.pregnancy = 1.0; // month 1 of gestation
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Phase 7 – New couple formation
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::phaseCoupleFormation() {
    auto& rng = RNG::get();

    // ── Collect eligible singles (age >= 12, status == Single) ───────────────
    std::vector<int> single_men, single_women;
    for (const auto& [id, p] : people_) {
        if (!p.isSingle() || p.age < 12.0) continue;
        if (p.isMale())   single_men.push_back(id);
        else              single_women.push_back(id);
    }

    // ── Filter by "wants a partner this month" ────────────────────────────────
    auto filterWilling = [&](const std::vector<int>& candidates) {
        std::vector<int> willing;
        willing.reserve(candidates.size());
        for (int id : candidates) {
            double prob = Tables::wantPartnerProbMonthly(people_.at(id).age);
            if (rng.uniform() < prob) willing.push_back(id);
        }
        return willing;
    };

    std::vector<int> men_willing   = filterWilling(single_men);
    std::vector<int> women_willing = filterWilling(single_women);

    // ── Shuffle both lists for unbiased random matching ───────────────────────
    std::shuffle(men_willing.begin(),   men_willing.end(),   rng.engine());
    std::shuffle(women_willing.begin(), women_willing.end(), rng.engine());

    // ── Greedy matching: each man tries each woman in random order ────────────
    std::unordered_set<int> paired_ids; // tracks already-paired individuals

    for (int mid : men_willing) {
        if (paired_ids.count(mid)) continue;
        if (!people_.count(mid))   continue;
        Person& man = people_.at(mid);
        if (!man.isSingle()) continue; // state may have changed

        for (int wid : women_willing) {
            if (paired_ids.count(wid)) continue;
            if (!people_.count(wid))   continue;
            Person& woman = people_.at(wid);
            if (!woman.isSingle()) continue;

            double diff = std::abs(man.age - woman.age);
            double prob = Tables::coupleFormProb(diff);

            if (rng.uniform() < prob) {
                // ── Form the couple ───────────────────────────────────────────
                man.status   = Status::Coupled;
                man.partner_id = wid;
                woman.status   = Status::Coupled;
                woman.partner_id = mid;
                paired_ids.insert(mid);
                paired_ids.insert(wid);
                ++yr_couples_formed_;
                break; // this man is taken – move to next man
            }
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Helpers
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::enterMourning(Person& p) {
    p.status        = Status::Mourning;
    p.partner_id    = -1;
    double mean     = Tables::mourningMeanMonths(p.age);
    p.mourning_months = RNG::get().exponential(mean);
    if (p.mourning_months < 1.0) p.mourning_months = 1.0; // at least one month
}

void Simulation::dissolveCouple(int id1, int id2, bool deathOfId1) {
    auto it2 = people_.find(id2);
    if (it2 != people_.end()) {
        enterMourning(it2->second);
    }

    if (!deathOfId1) {
        auto it1 = people_.find(id1);
        if (it1 != people_.end()) {
            enterMourning(it1->second);
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Statistics snapshot
// ═════════════════════════════════════════════════════════════════════════════
void Simulation::recordYear(int year) {
    int total = 0, males = 0, females = 0;
    int active_couples   = 0;
    int total_children   = 0;
    int num_women        = 0;

    for (const auto& [id, p] : people_) {
        ++total;
        if (p.isMale()) {
            ++males;
        } else {
            ++females;
            ++num_women;
            total_children += p.children_had;
            // Count each couple once via the female partner
            if (p.isCoupled() && p.partner_id != -1) {
                ++active_couples;
            }
        }
    }

    YearSnapshot snap;
    snap.year                   = year;
    snap.total_pop              = total;
    snap.males                  = males;
    snap.females                = females;
    snap.births                 = yr_births_;
    snap.deaths                 = yr_deaths_;
    snap.couples_formed         = yr_couples_formed_;
    snap.breakups               = yr_breakups_;
    snap.active_couples         = active_couples;
    snap.avg_children_per_woman = (num_women > 0)
                                  ? static_cast<double>(total_children) / num_women
                                  : 0.0;

    stats_.record(snap);
}