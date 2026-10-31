#include "tracking.hpp"

#include <cmath>

double triplet_score(const Hit& first, const Hit& middle, const Hit& last) {
    // For equally spaced layers, a straight path has
    // z_middle = (z_first + z_last) / 2, i.e. z_first - 2*z_middle + z_last = 0.
    return std::abs(first.z - 2.0 * middle.z + last.z);
}
