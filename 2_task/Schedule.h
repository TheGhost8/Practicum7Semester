#include <iostream>
#include <vector>
#include <map>
#include <set>

#pragma once

struct Comparator {
    bool operator()(const std::pair<size_t, size_t> &a, const std::pair<size_t, size_t> &b) const {
        if (a.first == b.first) {
            return a.second < b.second;
        }
        return  a.first < b.first;
    }
};

using graph = std::map<std::pair<size_t, size_t>, std::set<size_t>, Comparator>;

class Schedule
{
private:
    graph jobGraph;
    size_t energy;

public:
    std::vector<std::vector<std::pair<size_t, size_t>>> schedule;

    Schedule(size_t procs);
    Schedule(graph newJobGraph, size_t procs);
    Schedule(const Schedule &copied_schedule);

    size_t getEnergy() const {
        return energy;
    }

    void computeStateEnergy();

    bool secondIsFirstsParentJob(std::pair<size_t, size_t> first_job, size_t second_job) {
        return jobGraph[first_job].find(second_job) != jobGraph[first_job].end();
    }

    void clearSchedule();

    bool scheduleCorrect();

    bool operator==(const Schedule& compared_schedule);

    Schedule& operator=(const Schedule& copied_schedule);
};
