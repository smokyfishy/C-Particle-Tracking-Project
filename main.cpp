#include <cstdio>

#include "tracking.hpp"

int main() {
    const Layer inner{{{10, 0.125}, {11, 0.5}, {12, -0.25}}};
    const Hit middle{20, 0.25};
    const Layer outer{{{30, 0.375}, {31, 0.875}, {32, -0.5}}};

    // Largest score we still treat as a straight track.
    const double max_score = 0.1;

    Candidate best = select_best(inner, middle, outer);
    bool accepted = is_accepted(best, max_score);

    std::printf("first_id,middle_id,last_id,score,accepted\n");
    std::printf("%d,%d,%d,%f,%s\n", best.first_id, best.middle_id, best.last_id, best.score,
                accepted ? "yes" : "no");
    return 0;
}
