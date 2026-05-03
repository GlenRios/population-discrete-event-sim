#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Person.h  –  Attributes of every individual in the simulation
// ─────────────────────────────────────────────────────────────────────────────

enum class Sex    { Male, Female };
enum class Status { Single, Coupled, Mourning };

struct Person {
    int    id               = -1;
    Sex    sex              = Sex::Male;
    double age              = 0.0;   // years
    double death_age        = 0.0;   // years – set at birth/init
    int    desired_children = 0;
    int    children_had     = 0;
    Status status           = Status::Single;
    int    partner_id       = -1;    // -1 means no partner
    double mourning_months  = 0.0;   // months remaining in mourning
    double pregnancy        = 0.0;   // months pregnant (0 = not pregnant)

    // ── Convenience predicates ──────────────────────────────────────────────
    bool isMale()            const { return sex == Sex::Male;              }
    bool isFemale()          const { return sex == Sex::Female;            }
    bool isSingle()          const { return status == Status::Single;      }
    bool isCoupled()         const { return status == Status::Coupled;     }
    bool isMourning()        const { return status == Status::Mourning;    }
    bool isPregnant()        const { return pregnancy > 0.0;               }
    bool wantsMoreChildren() const { return children_had < desired_children; }
};