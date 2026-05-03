#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Tables.h  –  All probability tables from the model specification.
//              Each function either returns a probability or samples a value.
// ─────────────────────────────────────────────────────────────────────────────

#include "Person.h"

namespace Tables {

// ── 8.1  Desired number of children ─────────────────────────────────────────
/// Samples from the corrected (normalised) discrete distribution.
/// Returns a value in {1, 2, 3, 4, 5, 6} where 6 means "more than 5".
int sampleDesiredChildren();

// ── 8.2  Death age ───────────────────────────────────────────────────────────
/// Generates age-at-death for a person of the given sex using the
/// conditional mortality table and uniform sampling within each range.
double generateDeathAge(Sex sex);

// ── 8.3  Mourning duration ───────────────────────────────────────────────────
/// Returns the mean mourning time (months) for a person of the given age,
/// to be used as the parameter of an Exp(1/mean) distribution.
double mourningMeanMonths(double age);

// ── 8.4  Multiple births ─────────────────────────────────────────────────────
/// Samples number of babies born in a single delivery (1 – 5).
int sampleBirthCount();

// ── 8.5  Couple break-up ─────────────────────────────────────────────────────
/// Monthly break-up probability derived from the annual rate of 0.20.
double breakupProbMonthly();

// ── 8.6  Pregnancy ───────────────────────────────────────────────────────────
/// Monthly probability of conception for a woman of the given age.
/// Returns 0 for ages below 12.
double pregnancyProbMonthly(double age);

// ── 8.7  Desire for a partner ────────────────────────────────────────────────
/// Monthly probability that a single/post-mourning person of the given age
/// is "looking" for a partner this month.
double wantPartnerProbMonthly(double age);

// ── 8.8  Couple formation by age difference ──────────────────────────────────
/// Probability that two "willing" individuals form a couple given the
/// absolute age difference between them (years).
double coupleFormProb(double ageDiffYears);

} // namespace Tables