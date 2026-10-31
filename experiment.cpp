// Experiment: how often does select_best pick the true track as the
// measurement noise grows?
//
// Each synthetic event has one real straight track plus two random fake hits
// in both the inner and outer layers. Every real hit is smeared by Gaussian
// noise. We count how often the selected IDs match the real track.

#include <cstdio>
#include <random>

#include "tracking.hpp"

int main() {
    const int events_per_point = 10000;
    const double max_score = 0.1;
    const double noise_levels[] = {0.0, 0.01, 0.02, 0.05, 0.1, 0.2};

    std::mt19937 rng(42);  // fixed seed so results are reproducible
    std::uniform_real_distribution<double> start_dist(-1.0, 1.0);
    std::uniform_real_distribution<double> step_dist(-0.5, 0.5);
    std::uniform_real_distribution<double> fake_dist(-2.0, 2.0);
    std::uniform_int_distribution<int> slot_dist(0, kHitsPerLayer - 1);
    std::normal_distribution<double> gauss(0.0, 1.0);

    std::printf("noise,correct_fraction,accepted_fraction\n");

    for (double noise : noise_levels) {
        int correct = 0;
        int accepted = 0;

        for (int event = 0; event < events_per_point; ++event) {
            // True straight path: z = start + step * layer_index.
            double start = start_dist(rng);
            double step = step_dist(rng);

            // Fill both layers with fake hits, then hide the real hit
            // in a random slot.
            Layer inner;
            Layer outer;
            for (std::size_t i = 0; i < kHitsPerLayer; ++i) {
                inner[i] = {10 + static_cast<int>(i), fake_dist(rng)};
                outer[i] = {30 + static_cast<int>(i), fake_dist(rng)};
            }
            int true_inner = slot_dist(rng);
            int true_outer = slot_dist(rng);
            inner[true_inner].z = start + noise * gauss(rng);
            outer[true_outer].z = start + 2.0 * step + noise * gauss(rng);
            const Hit middle{20, start + step + noise * gauss(rng)};

            Candidate best = select_best(inner, middle, outer);

            if (best.first_id == inner[true_inner].id && best.last_id == outer[true_outer].id) {
                ++correct;
            }
            if (is_accepted(best, max_score)) {
                ++accepted;
            }
        }

        std::printf("%.2f,%.4f,%.4f\n", noise,
                    static_cast<double>(correct) / events_per_point,
                    static_cast<double>(accepted) / events_per_point);
    }

    return 0;
}
