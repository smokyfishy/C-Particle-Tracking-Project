# C++ Particle Tracking Project

When a particle flies through a detector, it leaves a trail of hits. The tricky
part is figuring out which hits belong to the same particle. This is a small
C++ program I wrote to try a simple version of that problem.

## The idea

Picture three detector layers, evenly spaced:

- The inner layer has 3 hits
- The middle layer has 1 hit
- The outer layer has 3 hits

I want to pick one inner hit and one outer hit that make a straight line with
the middle hit. That's 3 x 3 = 9 combinations to try.

Each hit is just an ID and a position:

```cpp
struct Hit {
    int id;
    double z;
};
```

## How it decides

If three points are on a straight line, the middle one sits exactly halfway
between the other two. So for each combination I calculate:

```
score = | z_first - 2 * z_middle + z_last |
```

A score of 0 means a perfect straight line, and bigger means more bent. The
program tries all 9 combinations and keeps the one with the lowest score.

## Example

| Layer  | Hits (ID, z)                           |
|--------|----------------------------------------|
| Inner  | (10, 0.125), (11, 0.5), (12, -0.25)    |
| Middle | (20, 0.25)                             |
| Outer  | (30, 0.375), (31, 0.875), (32, -0.5)   |

Hits 10, 20 and 30 go up by 0.125 each step, so they make a perfect line:

```
first_id,middle_id,last_id,score,accepted
10,20,30,0.000000,yes
```

## Running it

I built this on Linux with g++.

```bash
make run          # run the example
make test         # run the tests
make experiment   # run the noise experiment
```

## What's in here

- `tracking.hpp` / `tracking.cpp`: the main code (scoring and picking the best)
- `main.cpp`: runs the example above
- `test_tracking.cpp`: tests to make sure everything works
- `experiment.cpp`: the noise experiment described below

## Things I added

**A cutoff for bad matches.** The program always picks *something*, even if
every combination is bad. So I added a limit: if the best score is above 0.1,
the match gets rejected.

**A noise experiment.** Real detectors aren't perfect. Each measurement is
off by a little bit. I wanted to see how much that hurts, so I made 10,000
fake events for each noise level. Each event has one real straight track plus
some random fake hits. Then I counted how often the program found the real
track.

| How much noise | Found the real track |
|----------------|----------------------|
| none           | 100%                 |
| 0.01           | 93.5%                |
| 0.02           | 87.4%                |
| 0.05           | 73.9%                |
| 0.10           | 57.2%                |
| 0.20           | 38.3%                |

What surprised me:

- Even a tiny bit of noise causes mistakes. Sometimes the fake hits happen to
  line up better than the real ones.
- My cutoff didn't help with that. When a fake match is just as straight as a
  real one, the score can't tell them apart.

## What it doesn't do (yet)

This is a simplified learning project with made-up data. It uses one
coordinate, three layers and only a few hits. It doesn't use real detector
data or run on special hardware.

Some things I'd like to try next:

- Keep the top two matches and see how close they are
- Use lower-precision numbers and check whether the answers change. This
  matters for running tracking on hardware like FPGAs.
