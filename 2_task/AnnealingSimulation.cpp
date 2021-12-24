//
// Created by ghost on 31.10.2021.
//

#include "AnnealingSimulation.h"

AnnealingSimulation::AnnealingSimulation(graph &startGraph, size_t procs) {
    current_schedule = std::make_unique<Schedule>(startGraph, procs);
    best_schedule = std::make_unique<Schedule>(startGraph, procs);
    new_schedule = std::make_unique<Schedule>(procs);
}
/*
AnnealingSimulation::AnnealingSimulation(const AnnealingSimulation &copied_simulation) {
    current_schedule = std::make_unique<Schedule>(*copied_simulation.current_schedule);
    best_schedule = std::make_unique<Schedule>(*copied_simulation.best_schedule);
    new_schedule = std::make_unique<Schedule>(*copied_simulation.new_schedule);
}
*/
void AnnealingSimulation::start() {
    std::mt19937 twisterEngine;
    twisterEngine.seed(time(NULL));
    std::uniform_real_distribution<double> realDistribution(0, 1);

    size_t stepsFromLastChange = 0, i = 0;
    int64_t deltaEnergy = 0;

    while ((stepsFromLastChange < max_iterations_with_no_changes) && (i < max_iterations)) {
        for (size_t j = 0; j < MAX_ITERATION_IN; j++) {
            do {
                *new_schedule = *current_schedule;
                mutate();
            }
            while (!new_schedule->scheduleCorrect());

            deltaEnergy = (int64_t)new_schedule->getEnergy() - (int64_t)current_schedule->getEnergy();

            if ((deltaEnergy < 0) || (realDistribution(twisterEngine) < calculateProbability(deltaEnergy, ((double)start_temperature / std::log(1 + i))))) {
                *current_schedule = *new_schedule;
            }
            if (current_schedule->getEnergy() < best_schedule->getEnergy()) {
                *best_schedule = *current_schedule;
                stepsFromLastChange = 0;
            }
        }
        ++stepsFromLastChange;
        ++i;
    }

    //printBestSchedule();
    //std::cout << "Best energy: " << std::to_string(best_schedule->getEnergy()) << std::endl;
}

void AnnealingSimulation::mutate() {
    std::mt19937 twisterEngine;
    twisterEngine.seed(time(NULL));

    size_t proc_1 = 0, proc_2 = 0, taskToMove = 0; //, new_position = 0, start_time = 0, min_start_time = 0;

    std::uniform_int_distribution<size_t> uniformDistributionProc(0, new_schedule->schedule.size() - 1);

    do {
        proc_1 = uniformDistributionProc(twisterEngine);
        proc_2 = uniformDistributionProc(twisterEngine);
    }
    while (new_schedule->schedule[proc_1].empty());

    std::uniform_int_distribution<size_t> uniformDistributionTask(0, new_schedule->schedule[proc_1].size() - 1);
    taskToMove = uniformDistributionTask(twisterEngine);

    if (proc_1 == proc_2) {
        firstTypeMutation(proc_1, taskToMove);
    }
    else {
        secondTypeMutation(proc_1, proc_2, taskToMove);
    }
    new_schedule->clearSchedule();

    new_schedule->computeStateEnergy();

    /*


    for (auto &proc : new_schedule->schedule) {
        start_time = 0;
        for (auto &job : proc) {
            start_time += job.second;
            if (new_schedule->secondIsFirstsParentJob(new_schedule->schedule[proc_1][taskToMove], job.first) &&
                (start_time > min_start_time)) {
                min_start_time = start_time;
            }
        }
    }
    start_time = 0;
    new_position = 0;
    while ((new_position < new_schedule->schedule[proc_2].size()) &&
           (start_time < min_start_time)) {
        start_time += new_schedule->schedule[proc_2][new_position].second;
        ++new_position;
    }
    if ((new_position == new_schedule->schedule[proc_2].size()) && (start_time < min_start_time)) {
        if (new_schedule->schedule[proc_2].empty() || (new_schedule->schedule[proc_2].back().first != 0)) {
            new_schedule->schedule[proc_2].push_back(std::make_pair(0, min_start_time - start_time));
            ++new_position;
        }
        else {
            new_schedule->schedule[proc_2][new_schedule->schedule[proc_2].size() - 1].second += min_start_time - start_time;
        }
    }
    if ((new_position > 0) && (new_schedule->schedule[proc_2][new_position-1].first == 0)) {
        if ((new_schedule->schedule[proc_2][new_position-1].second - new_schedule->schedule[proc_1][taskToMove].second < min_start_time) && start_time) {
            new_schedule->schedule[proc_2][new_position-1].second = min_start_time - (start_time - new_schedule->schedule[proc_2][new_position].second);
        }
        else {
            new_schedule->schedule[proc_2][new_position-1].second -= new_schedule->schedule[proc_1][taskToMove].second;
        }
    }
    if ((!new_schedule->schedule[proc_2].empty()) && (new_position < (new_schedule->schedule[proc_2].size() - 1)) && (new_schedule->schedule[proc_2][new_position+1].first == 0)) {
        if (new_schedule->schedule[proc_1][taskToMove].second >= new_schedule->schedule[proc_2][new_position+1].second) {
            new_schedule->schedule[proc_2].erase(new_schedule->schedule[proc_2].begin() + new_position + 1);
        }
        else {
            new_schedule->schedule[proc_2][new_position+1].second -= new_schedule->schedule[proc_1][taskToMove].second;
        }
    }

    new_schedule->schedule[proc_2].insert(new_schedule->schedule[proc_2].begin() + new_position, new_schedule->schedule[proc_1][taskToMove]);
    new_schedule->schedule[proc_1].erase(new_schedule->schedule[proc_1].begin() + taskToMove);
    if (taskToMove < new_schedule->schedule[proc_1].size() - 1) {
        new_schedule->schedule[proc_1].insert(new_schedule->schedule[proc_1].begin() + taskToMove, std::make_pair(0, new_schedule->schedule[proc_2][new_position].second));
    }

    new_schedule->computeStateEnergy();
    */
}

void AnnealingSimulation::firstTypeMutation(size_t proc, size_t taskToMove) {
    size_t start_time = 0, min_start_time = 0, leftJobBorder = 0, taskToInsert = 0;
    auto saved_task = new_schedule->schedule[proc][taskToMove];
    std::mt19937 twisterEngine;
    twisterEngine.seed(time(NULL));

    for (auto &p : new_schedule->schedule) {
        start_time = 0;
        for (auto &j : p) {
            start_time += j.second;
            if (new_schedule->secondIsFirstsParentJob(new_schedule->schedule[proc][taskToMove], j.first) &&
                (start_time > min_start_time)) {
                min_start_time = start_time;
            }
        }
    }

    start_time = 0;
    for (leftJobBorder = 0; leftJobBorder < new_schedule->schedule[proc].size(); ++leftJobBorder) {
        if (leftJobBorder == taskToMove) {
            continue;
        }
        if (start_time >= min_start_time) {
            break;
        }
        start_time += new_schedule->schedule[proc][leftJobBorder].second;
    }
    if (leftJobBorder < new_schedule->schedule[proc].size()) {
        new_schedule->schedule[proc].erase(new_schedule->schedule[proc].begin() + taskToMove);
        std::uniform_int_distribution<size_t> uniformDistributionTask(leftJobBorder, new_schedule->schedule[proc].size());
        taskToInsert = uniformDistributionTask(twisterEngine);
        if (taskToInsert == new_schedule->schedule[proc].size()) {
            new_schedule->schedule[proc].push_back(saved_task);
        }
        else {
            new_schedule->schedule[proc].insert(new_schedule->schedule[proc].begin() + taskToInsert, saved_task);
        }
    }
}

void AnnealingSimulation::secondTypeMutation(size_t procRemove, size_t procInsert, size_t taskToMove) {
    size_t min_start_time = 0, start_time = 0, job = 0;

    for (job = 0; job < taskToMove; ++job) {
        min_start_time += new_schedule->schedule[procRemove][job].second;
    }

    start_time = 0;
    for (job = 0;  job < new_schedule->schedule[procInsert].size(); ++job) {
        if (min_start_time <= start_time) {
            break;
        }
        start_time += new_schedule->schedule[procInsert][job].second;
    }
    if ((job == new_schedule->schedule[procInsert].size()) && (start_time < min_start_time)) {
        if ((!new_schedule->schedule[procInsert].empty()) && (new_schedule->schedule[procInsert].back().first == 0)) {
            new_schedule->schedule[procInsert].back().second += min_start_time - start_time;
        }
        else {
            new_schedule->schedule[procInsert].push_back(std::make_pair(0, min_start_time - start_time));
        }
        new_schedule->schedule[procInsert].push_back(new_schedule->schedule[procRemove][taskToMove]);
        new_schedule->schedule[procRemove].erase(new_schedule->schedule[procRemove].begin() + taskToMove);
    }
    else {
        new_schedule->schedule[procInsert].insert(new_schedule->schedule[procInsert].begin() + job, new_schedule->schedule[procRemove][taskToMove]);
        new_schedule->schedule[procRemove].erase(new_schedule->schedule[procRemove].begin() + taskToMove);
    }
}

void AnnealingSimulation::printBestSchedule() {
    for (size_t proc = 0; proc < best_schedule->schedule.size(); ++proc) {
        std::cout << std::to_string(proc) << " proc: ";
        for (auto &job : best_schedule->schedule[proc]) {
            std::cout << std::to_string(job.first) << " (" << std::to_string(job.second) << ") ";
        }
        std::cout << std::endl;
    }
}

void AnnealingSimulation::printCurrentSchedule() {
    for (size_t proc = 0; proc < current_schedule->schedule.size(); ++proc) {
        std::cout << std::to_string(proc) << " proc: ";
        for (auto &job : current_schedule->schedule[proc]) {
            std::cout << std::to_string(job.first) << " ";
        }
        std::cout << std::endl;
    }
}

AnnealingSimulation &AnnealingSimulation::operator=(const AnnealingSimulation &copied_simulation) {
    *best_schedule = *copied_simulation.best_schedule;
    *current_schedule = *copied_simulation.current_schedule;
    *new_schedule = *copied_simulation.new_schedule;
}

void AnnealingSimulation::updateSchedules(const AnnealingSimulation &best_simulation) {
    *best_schedule = *best_simulation.best_schedule;
    *current_schedule = *best_simulation.best_schedule;
}
