# population-discrete-event-sim

A modular **discrete-event population simulation** written in C++17.  
The model advances time in **monthly steps** for up to 100 simulated years and tracks birth, death, couple formation, pregnancy, mourning, and break-ups across a synthetic population.

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Build & Run](#build--run)
- [Command-Line Arguments](#command-line-arguments)
- [Model Description](#model-description)
  - [Initialization](#initialization)
  - [Monthly Phases](#monthly-phases)
  - [Probability Tables](#probability-tables)
- [Output](#output)
- [Extending the Simulation](#extending-the-simulation)

---

## Features

| Feature | Detail |
|---------|--------|
| Time resolution | 1 month (1/12 year) |
| Simulation horizon | Configurable (default 100 years) |
| Mortality | Age- and sex-specific, 4 conditional ranges |
| Fertility | Age-specific monthly probability, multiple births supported |
| Partnership | Formation probability depends on age difference; annual break-up rate |
| Mourning | Exponential duration with age-dependent mean |
| Output | Console table + `results.csv` |
| Reproducibility | Deterministic with fixed RNG seed |

---

## Project Structure

```
population_sim/
├── README.md
├── include/
│   ├── Person.h        # Individual agent (attributes + predicates)
│   ├── Random.h        # Singleton RNG wrapper (Mersenne Twister 64-bit)
│   ├── Tables.h        # All probability/sampling functions
│   ├── Statistics.h    # Yearly snapshot data structure and reporter
│   └── Simulation.h    # Main engine (owns population, drives monthly loop)
└── src/
    ├── main.cpp        # Entry point – CLI parsing, banner, run
    ├── Random.cpp
    ├── Tables.cpp
    ├── Statistics.cpp
    └── Simulation.cpp
```

---

## Requirements

- **C++17** compatible compiler — any of the following:
  - **Windows:** [MinGW-w64 / GCC](https://winlibs.com) o Visual Studio 2017+
  - **Linux:** GCC ≥ 7 (`sudo apt install g++`)
  - **macOS:** Clang ≥ 5 (`xcode-select --install`)

---

## Build & Run

```bash
# Compile
make

# Run with defaults (500 males, 500 females, 100 years, seed 42)
./population_sim

# or use the Makefile shortcut
make run
```

### Clean

```bash
make clean
```

### Debug build (no optimisations, debug symbols)

```bash
make debug
```

---

## Command-Line Arguments

```
./population_sim [males] [females] [years] [seed]
```

| Argument | Default | Description |
|----------|---------|-------------|
| `males`   | 500 | Initial number of male individuals |
| `females` | 500 | Initial number of female individuals |
| `years`   | 100 | Simulation duration in years |
| `seed`    | 42  | RNG seed (same seed → identical run) |

**Examples**

```bash
# 1 000 individuals, 50 years
./population_sim 500 500 50

# Larger population, custom seed
./population_sim 2000 2000 100 1234
```

---

## Model Description

### Initialization

All initial individuals receive:

- **Age** – drawn uniformly from `[0, 100)` years.
- **Sex** – specified by the `males` / `females` counts.
- **Death age** – sampled from the age- and sex-specific mortality table (see below).
- **Desired children** – sampled from the normalised discrete distribution.
- All start as *single*, with no partner and no pregnancy.

### Monthly Phases

Each simulated month the engine executes seven phases **in order**:

| # | Phase | Key logic |
|---|-------|-----------|
| 1 | **Update ages** | Every individual's age increases by 1/12 year |
| 2 | **Deaths** | Any individual whose `age ≥ death_age` is removed; surviving partners enter mourning |
| 3 | **Births** | Gestational age advances; at ≥ 9 months a delivery occurs (1–5 babies) |
| 4 | **End of mourning** | Mourning counter decrements; those reaching 0 become single |
| 5 | **Break-ups** | Each couple faces a monthly break-up probability ≈ 1.84 % |
| 6 | **Pregnancies** | Coupled, non-pregnant women under fertility conditions may conceive |
| 7 | **Couple formation** | Willing singles are shuffled and matched by an age-difference probability |

### Probability Tables

#### Desired children (normalised from original weights)

| # children | Weight | Normalised |
|-----------|--------|-----------|
| 1 | 0.60 | 0.293 |
| 2 | 0.75 | 0.366 |
| 3 | 0.35 | 0.171 |
| 4 | 0.20 | 0.098 |
| 5 | 0.10 | 0.049 |
| >5 | 0.05 | 0.024 |

#### Mortality – conditional probability per range

| Age range | Male | Female |
|-----------|------|--------|
| 0 – 12    | 0.25 | 0.25 |
| 12 – 45   | 0.10 | 0.15 |
| 45 – 76   | 0.30 | 0.35 |
| 76 – 125  | 0.70 | 0.65 |

#### Mourning duration mean (months)

| Age range | Mean |
|-----------|------|
| 12 – 15   | 3  |
| 15 – 35   | 6  |
| 35 – 45   | 12 |
| 45 – 60   | 24 |
| 60 – 125  | 48 |

#### Monthly pregnancy probability (converted from annual)

| Age | p_annual | p_monthly |
|-----|----------|-----------|
| 12–15 | 0.20 | 0.0184 |
| 15–21 | 0.45 | 0.0471 |
| 21–35 | 0.80 | 0.1161 |
| 35–45 | 0.40 | 0.0404 |
| 45–60 | 0.20 | 0.0184 |
| 60+ | 0.05 | 0.0043 |

#### Couple formation probability by age difference

| Δ age (years) | Probability |
|---------------|-------------|
| 0 – 5   | 0.45 |
| 5 – 10  | 0.40 |
| 10 – 15 | 0.35 |
| 15 – 20 | 0.25 |
| ≥ 20    | 0.15 |

---

## Output

### Console

A formatted table is printed after the run:

```
  Year       Pop     Males   Females    Births    Deaths  NewPairs  Breakups     Pairs  AvgKids/W
-----------------------------------------------------------------------------------------------------
     1       984       492       492       112        16        41        30        57       0.11
     2       982       489       493       109        11        38        27        64       0.22
   ...
```

### CSV (`results.csv`)

All yearly snapshots are saved in comma-separated format:

```
year,total_pop,males,females,births,deaths,couples_formed,breakups,active_couples,avg_children_per_woman
1,984,492,492,112,16,41,30,57,0.1100
...
```

You can open this directly in Excel, LibreOffice Calc, or plot it with Python/matplotlib.

---

## Extending the Simulation

| Goal | Where to change |
|------|-----------------|
| Tweak any probability | `src/Tables.cpp` |
| Add attributes to individuals | `include/Person.h` |
| Add a new monthly phase | `include/Simulation.h` + `src/Simulation.cpp` |
| Track additional statistics | `include/Statistics.h` + `src/Statistics.cpp` + `Simulation::recordYear()` |
| Change CSV columns | `Statistics::saveCSV()` |