#include "tracking.hpp"

#include <cmath>
#include <limits>

double triplet_score(const Hit& first, const Hit& middle, const Hit& last) {
    // For equally spaced layers, a straight path has
    // z_middle = (z_first + z_last) / 2, i.e. z_first - 2*z_middle + z_last = 0.
    return std::abs(first.z - 2.0 * middle.z + last.z);
}

Candidate select_best(const Layer& inner, const Hit& middle, const Layer& outer) {
    // Start with an "infinitely bad" score so the first triplet always wins.
    Candidate best{-1, -1, -1, std::numeric_limits<double>::infinity()};

    for (const Hit& first : inner) {
        for (const Hit& last : outer) {
            double score = triplet_score(first, middle, last);

            // Strictly smaller, so an exact tie keeps the earlier candidate.
            if (score < best.score) {
                best = {first.id, middle.id, last.id, score};
            }
        }
    }

    return best;
}
