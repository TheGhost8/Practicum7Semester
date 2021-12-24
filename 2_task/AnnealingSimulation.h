#include <iostream>
#include <random>
#include <memory>
#include "Schedule.h"

#pragma once

static const size_t start_temperature = 10E5;
static const size_t max_iterations = 100;
static const size_t max_iterations_with_no_changes = 100;
static const size_t MAX_ITERATION_IN = 100;

class AnnealingSimulation
{
private:
    std::unique_ptr<Schedule> current_schedule;
    std::unique_ptr<Schedule> new_schedule;
    std::unique_ptr<Schedule> best_schedule;

public:
    AnnealingSimulation(graph &startGraph, size_t procs);
    AnnealingSimulation(const AnnealingSimulation &copied_simulation);

    size_t getBestEnergy() { return best_schedule->getEnergy(); };
    size_t getCurrentEnergy() { return current_schedule->getEnergy(); };

    double calculateProbability(double dE, double t) {
        return std::exp(-dE / t);
    }

    void start();
    void mutate();
    void firstTypeMutation(size_t proc, size_t taskToMove);
    void secondTypeMutation(size_t procRemove, size_t procInsert, size_t taskToMove);

    void printBestSchedule();
    void printCurrentSchedule();

    AnnealingSimulation& operator=(const AnnealingSimulation& copied_simulation);

    void updateSchedules(const AnnealingSimulation &best_simulation);
};
