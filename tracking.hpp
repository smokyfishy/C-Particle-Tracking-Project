#ifndef TRACKING_HPP
#define TRACKING_HPP

#include <array>
#include <cstddef>

// One detector measurement: an ID and a position along z.
struct Hit {
    int id;
    double z;
};

// The best triplet found: the three hit IDs and their score.
struct Candidate {
    int first_id;
    int middle_id;
    int last_id;
    double score;
};

// Each of the inner and outer layers holds a fixed number of hits.
constexpr std::size_t kHitsPerLayer = 3;
using Layer = std::array<Hit, kHitsPerLayer>;

// Straight-line mismatch for one triplet: |z_first - 2*z_middle + z_last|.
// Smaller is better; 0 means the three hits lie on a perfect straight line.
double triplet_score(const Hit& first, const Hit& middle, const Hit& last);

// Tries every (inner, middle, outer) combination and returns the one with
// the smallest score. Exact ties keep the first candidate encountered.
Candidate select_best(const Layer& inner, const Hit& middle, const Layer& outer);

// The search above always returns *something*, even if every triplet is bad.
// This lets the caller reject candidates whose score is above max_score.
bool is_accepted(const Candidate& candidate, double max_score);

#endif
