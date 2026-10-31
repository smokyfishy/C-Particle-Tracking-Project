#include <cmath>
#include <cstdio>

#include "tracking.hpp"

// A tiny test helper: print each failure and count them.
static int failures = 0;

static void check(bool condition, const char* name) {
    if (!condition) {
        std::printf("FAIL: %s\n", name);
        ++failures;
    }
}

static bool close(double a, double b) {
    return std::abs(a - b) < 1e-12;
}

static void test_straight_triplet_scores_zero() {
    check(close(triplet_score({1, 0.125}, {2, 0.25}, {3, 0.375}), 0.0),
          "straight triplet scores zero");
}

static void test_bent_triplet_scores_positive() {
    // |0.125 - 2*0.25 + 0.875| = 0.5
    check(close(triplet_score({1, 0.125}, {2, 0.25}, {3, 0.875}), 0.5),
          "bent triplet has the expected score");
}

static void test_negative_positions() {
    // A straight line through negative z: -0.5, -0.25, 0.0
    check(close(triplet_score({1, -0.5}, {2, -0.25}, {3, 0.0}), 0.0),
          "straight line with negative z scores zero");
    // |-0.25 - 2*0.25 + (-0.5)| = 1.25
    check(close(triplet_score({1, -0.25}, {2, 0.25}, {3, -0.5}), 1.25),
          "bent triplet with negative z has the expected score");
}

static void test_example_event_selects_10_20_30() {
    const Layer inner{{{10, 0.125}, {11, 0.5}, {12, -0.25}}};
    const Hit middle{20, 0.25};
    const Layer outer{{{30, 0.375}, {31, 0.875}, {32, -0.5}}};

    Candidate best = select_best(inner, middle, outer);

    check(best.first_id == 10, "example: first_id is 10");
    check(best.middle_id == 20, "example: middle_id is 20");
    check(best.last_id == 30, "example: last_id is 30");
    check(close(best.score, 0.0), "example: score is 0");
}

static void test_inputs_are_not_modified() {
    const Layer inner{{{10, 0.125}, {11, 0.5}, {12, -0.25}}};
    const Hit middle{20, 0.25};
    const Layer outer{{{30, 0.375}, {31, 0.875}, {32, -0.5}}};

    select_best(inner, middle, outer);

    check(inner[0].id == 10 && inner[0].z == 0.125, "inner hit 10 unchanged");
    check(middle.id == 20 && middle.z == 0.25, "middle hit unchanged");
    check(outer[0].id == 30 && outer[0].z == 0.375, "outer hit 30 unchanged");
}

static void test_tie_keeps_first_candidate() {
    // (10,20,30) and (11,20,31) are both perfect straight lines.
    const Layer inner{{{10, 0.0}, {11, 1.0}, {12, 5.0}}};
    const Hit middle{20, 1.0};
    const Layer outer{{{30, 2.0}, {31, 1.0}, {32, 9.0}}};

    Candidate best = select_best(inner, middle, outer);

    check(best.first_id == 10 && best.last_id == 30, "tie keeps first candidate");
}

static void test_shifting_all_hits_gives_same_answer() {
    // Moving every hit by the same amount should not change which triplet
    // looks straightest, or its score.
    const double shift = 3.0;
    const Layer inner{{{10, 0.125 + shift}, {11, 0.5 + shift}, {12, -0.25 + shift}}};
    const Hit middle{20, 0.25 + shift};
    const Layer outer{{{30, 0.375 + shift}, {31, 0.875 + shift}, {32, -0.5 + shift}}};

    Candidate best = select_best(inner, middle, outer);

    check(best.first_id == 10 && best.last_id == 30, "shifted event selects 10,20,30");
    check(close(best.score, 0.0), "shifted event score is 0");
}

int main() {
    test_straight_triplet_scores_zero();
    test_bent_triplet_scores_positive();
    test_negative_positions();
    test_example_event_selects_10_20_30();
    test_inputs_are_not_modified();
    test_tie_keeps_first_candidate();
    test_shifting_all_hits_gives_same_answer();

    if (failures == 0) {
        std::printf("All tests passed.\n");
        return 0;
    }
    std::printf("%d test(s) failed.\n", failures);
    return 1;
}
