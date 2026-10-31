# C++ Particle Tracking Project

A small C++ program that picks the combination of detector hits that best
fits a straight particle path across three layers.

It's a simplified learning project using synthetic data. I built it to
practice the software side of particle-tracking algorithms: representing
measurements, writing a small numerical algorithm, testing it, and measuring
how it behaves.

## The problem

A particle passes through three equally spaced detector layers. Each layer
records possible measurements called **hits**. The task is to find which hits
belong to the same particle.

- The **inner** layer has 3 possible hits.
- The **middle** layer has 1 already-selected hit.
- The **outer** layer has 3 possible hits.

That gives 3 × 3 = **9 possible triplets**. The program scores every one and
keeps the best.

Each hit has an ID and one position, `z`:

```cpp
struct Hit {
    int id;
    double z;
};
```

## How a triplet is scored

If the path is straight and the layers are equally spaced, the middle hit
should sit halfway between the other two:

```
z_middle = (z_first + z_last) / 2   =>   z_first - 2*z_middle + z_last = 0
```

So each triplet gets the score:

```
score = | z_first - 2*z_middle + z_last |
```

**Smaller is better.** A score of 0 means a perfect straight line. The score
measures geometric mismatch. It is not a probability.

## Example

| Layer  | Hits `(ID, z)`                          |
|--------|-----------------------------------------|
| Inner  | `(10, 0.125)`, `(11, 0.5)`, `(12, -0.25)` |
| Middle | `(20, 0.25)`                            |
| Outer  | `(30, 0.375)`, `(31, 0.875)`, `(32, -0.5)` |

Hits 10, 20, 30 step up by 0.125 each layer, so they fit perfectly:

```
first_id,middle_id,last_id,score,accepted
10,20,30,0.000000,yes
```

## Files

| File                | What it does                                              |
|---------------------|-----------------------------------------------------------|
| `tracking.hpp`      | Declares `Hit`, `Candidate`, `Layer` and the functions    |
| `tracking.cpp`      | Implements scoring, the search, and the threshold check   |
| `main.cpp`          | Runs the example event and prints the result              |
| `test_tracking.cpp` | Unit tests                                                |
| `experiment.cpp`    | Noise study on 60,000 synthetic events                    |
| `Makefile`          | Build, run, and test commands                             |

The core is two functions:

```cpp
double triplet_score(const Hit& first, const Hit& middle, const Hit& last);
Candidate select_best(const Layer& inner, const Hit& middle, const Layer& outer);
```

`select_best` uses two nested loops over the inner and outer layers. It keeps
a candidate only when its score is *strictly* smaller than the best so far,
so if two triplets tie exactly, the first one found wins. Inputs are passed
by `const` reference, so they aren't copied and can't be changed.

## Build and run (Linux)

```bash
make run          # build and run the example
make test         # build and run the unit tests
make experiment   # run the noise study, saves results/noise_study.csv
make clean
```

Or without `make`:

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp tracking.cpp -o build/tracker
./build/tracker
```

## Tests

`test_tracking.cpp` checks:

- straight triplets score 0 and bent ones score what they should
- negative positions
- the example selects hits 10, 20, 30 with score 0
- the input hits are not modified
- exact ties keep the first candidate
- shifting every hit by the same amount doesn't change the answer
- the threshold accepts good candidates and rejects poor ones

## Extension 1: rejecting poor candidates

`select_best` always returns *something*, even if every triplet is bad.
`is_accepted(candidate, max_score)` lets the caller reject a candidate whose
score is above a threshold (`max_score = 0.1` in `main.cpp`).

## Extension 2: noise experiment

Real detectors measure positions with some error. To see how the algorithm
copes, `experiment.cpp` generates 10,000 synthetic events per noise level.
Each event has:

- one real straight track, with a random start and slope
- two random fake hits in both the inner and outer layers
- Gaussian noise (standard deviation = `noise`) added to every real hit

It counts how often `select_best` picks the real track, and how often the
chosen candidate passes the 0.1 threshold. The random seed is fixed, so the
results are reproducible.

| Noise (σ) | Picked real track | Passed threshold |
|-----------|-------------------|------------------|
| 0.00      | 100.0%            | 100.0%           |
| 0.01      | 93.5%             | 100.0%           |
| 0.02      | 87.4%             | 97.3%            |
| 0.05      | 73.9%             | 71.0%            |
| 0.10      | 57.2%             | 50.8%            |
| 0.20      | 38.3%             | 40.8%            |

**What I learned:**

- With no noise, the real track scores 0 and always wins. Once noise is
  added, a triplet that includes a fake hit can line up by chance and score
  better than the real one. Accuracy falls quickly: about 1 in 15 events is
  wrong at σ = 0.01.
- At low noise, the threshold accepts nearly everything, *including the wrong
  picks*. A score threshold alone can't tell a real track from a lucky fake
  one that's just as straight.
- Choosing the threshold is a tradeoff. It should match the expected
  measurement noise, or it starts rejecting real tracks.

## Limitations

This runs on a CPU with one coordinate, three layers, and one fixed middle
hit. It doesn't reconstruct full detector events, use real data, or run on
hardware.

## Possible next steps

- Return the two best candidates and use the gap between their scores as a
  confidence measure.
- Reduce numerical precision (for example, `float` or fixed-point integers)
  and measure how often the selected candidate changes. This matters for
  FPGA implementations.
- Explore a hardware implementation and measure its resource use and latency.
