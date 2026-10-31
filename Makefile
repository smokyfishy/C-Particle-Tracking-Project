CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic

all: build/tracker build/test_tracking build/experiment

build/tracker: main.cpp tracking.cpp tracking.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) main.cpp tracking.cpp -o $@

build/test_tracking: test_tracking.cpp tracking.cpp tracking.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) test_tracking.cpp tracking.cpp -o $@

build/experiment: experiment.cpp tracking.cpp tracking.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) experiment.cpp tracking.cpp -o $@

run: build/tracker
	./build/tracker

test: build/test_tracking
	./build/test_tracking

experiment: build/experiment
	./build/experiment | tee results/noise_study.csv

clean:
	rm -rf build

.PHONY: all run test experiment clean
