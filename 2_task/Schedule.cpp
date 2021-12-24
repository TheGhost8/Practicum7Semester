//
// Created by ghost on 31.10.2021.
//

#include "Schedule.h"

using graph = std::map<std::pair<size_t, size_t>, std::set<size_t>, Comparator>;
/*
Schedule::Schedule(graph newJobGraph, char a) : jobGraph(newJobGraph) {
    schedule.reserve(4);
    schedule.push_back(std::vector<std::pair<size_t, size_t>>());
    schedule.push_back(std::vector<std::pair<size_t, size_t>>());
    schedule.push_back(std::vector<std::pair<size_t, size_t>>());
    schedule.push_back(std::vector<std::pair<size_t, size_t>>());
    schedule[1].push_back(std::make_pair(0, 111));
    schedule[1].push_back(std::make_pair(2, 130));
    schedule[2].push_back(std::make_pair(3, 111));
    schedule[2].push_back(std::make_pair(1, 131));
    schedule[3].push_back(std::make_pair(4, 77));
    schedule[3].push_back(std::make_pair(0, 77));
    schedule[3].push_back(std::make_pair(5, 85));

    scheduleCorrect();
}
*/
Schedule::Schedule(size_t procs) {
    schedule.reserve(procs);
    for (size_t i = 0; i < procs; ++i) {
        schedule.emplace_back(std::vector<std::pair<size_t, size_t>>());
    }
    computeStateEnergy();
}

Schedule::Schedule(graph newJobGraph, size_t procs) : jobGraph(newJobGraph) {
    bool has_jobs = true, is_scheduled = true;
    schedule.reserve(procs);
    for (size_t i = 0; i < procs; ++i) {
        schedule.emplace_back(std::vector<std::pair<size_t, size_t>>());
    }
    schedule[0].reserve(newJobGraph.size());

    for (auto &job : newJobGraph) {
        if (job.second.empty()) {
            schedule[0].push_back(job.first);
            has_jobs = true;
            break;
        }
    }
    for (auto &job : schedule[0]) {
        newJobGraph.erase(job);
    }

    while (!newJobGraph.empty()) {
        for (auto &job : newJobGraph) {
            has_jobs = true;
            for (auto &dependent_job : job.second) {
                is_scheduled = false;
                for (auto &scheduled_job : schedule[0]) {
                    if (scheduled_job.first == dependent_job) {
                        is_scheduled = true;
                    }
                }
                if (!is_scheduled) {
                    has_jobs = false;
                    break;
                }
            }
            if (has_jobs) {
                schedule[0].push_back(job.first);
                newJobGraph.erase(job.first);
                break;
            }
        }
    }
    /*
    for (auto &job : jobGraph) {
        std::cout << job.second.size() << std::endl;
    }
    */
    computeStateEnergy();
}
/*
Schedule::Schedule(const Schedule &copied_schedule) {
    jobGraph = copied_schedule.jobGraph;
    schedule = copied_schedule.schedule;
    energy = copied_schedule.energy;
}
*/
void Schedule::computeStateEnergy() {
    size_t newEnergy = 0, maxTime = 0, numEmptyProc = 0, currentTime = 0;
    for (auto &i : schedule) {
        if (i.empty()) {
            ++numEmptyProc;
        }
        currentTime = 0;
        for (auto &j : i) {
            currentTime += j.second;
            if (j.first == 0) {
                newEnergy += j.second;
            }
        }
        maxTime = (currentTime > maxTime ? currentTime : maxTime);
    }
    newEnergy += maxTime * numEmptyProc;
    energy = newEnergy;
}

void Schedule::clearSchedule() {
    for (auto &proc : schedule) {
        if ((!proc.empty()) && (proc.back().first == 0)) {
            proc.pop_back();
        }
    }
}

bool Schedule::scheduleCorrect() {
    size_t start_time = 0, work_time = 0;
    std::map<size_t, size_t> jobNumberTimes;
    std::map<std::pair<size_t, size_t>, std::pair<std::optional<size_t>, size_t>> jobTimes;

    for (auto &proc : schedule) {
        start_time = 0;
        for (auto &job : proc) {
            if (job.first != 0) {
                jobTimes[job].second = start_time;
                jobNumberTimes[job.first] = start_time;
            }
            start_time += job.second;
        }
    }

    for (auto &job : jobGraph) {
        start_time = 0;
        for (auto &dependentJob : job.second) {
            if (jobNumberTimes[dependentJob] > start_time) {
                start_time = jobNumberTimes[dependentJob];
                for (auto &job2 : jobGraph) {
                    if (job2.first.first == dependentJob) {
                        work_time = job2.first.second;
                    }
                }
            }
            else if (jobNumberTimes[dependentJob] == start_time) {
                for (auto &job2 : jobGraph) {
                    if ((job2.first.first == dependentJob) && (job2.first.second > work_time)) {
                        work_time = job2.first.second;
                    }
                }
            }
        }
        jobTimes[job.first].first = start_time + work_time;
        if (jobTimes[job.first].first > jobTimes[job.first].second) {
            return false;
        }
    }
    return true;
}

bool Schedule::operator==(const Schedule &compared_schedule) {
    if (schedule.size() != compared_schedule.schedule.size()) {
        return false;
    }
    for (size_t i = 0; i < schedule.size(); ++i) {
        if (schedule[i].size() != compared_schedule.schedule[i].size()) {
            return false;
        }
        for (size_t j = 0; j < schedule[i].size(); ++j) {
            if (schedule[i][j] != compared_schedule.schedule[i][j]) {
                return false;
            }
        }
    }
    return true;
}

Schedule &Schedule::operator=(const Schedule &copied_schedule) {
    if (*this == copied_schedule) {
        return *this;
    }
    jobGraph = copied_schedule.jobGraph;
    for (size_t i = 0; i < schedule.size(); i++) {
        schedule[i].clear();
        for (auto& j : copied_schedule.schedule[i]) {
            schedule[i].push_back(j);
        }
    }
    energy = copied_schedule.getEnergy();
    return *this;
}
