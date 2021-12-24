#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <thread>
#include "AnnealingSimulation.h"

using graph = std::map<std::pair<size_t, size_t>, std::set<size_t>, Comparator>;

void ReadJobGraphFromFile(graph &jobGraph, std::string file_name)
{
    std::string input;
    size_t jobsNum = 0, jobTime = 0;

    std::ifstream file(file_name);

    if (file.is_open()) {
        while (!((input[0] >= '0') && (input[0] <= '9'))) {
            std::getline(file, input);
        }
        jobsNum = std::stoi(input);
        printf("%zd\n", jobsNum);
        std::getline(file, input);
        while (!((input[0] >= '0') && (input[0] <= '9'))) {
            std::getline(file, input);
        }
        for (size_t i = 0; i < jobsNum; ++i) {
            if (!((input[0] >= '0') && (input[0] <= '9'))) {
                --i;
                std::getline(file, input);
                continue;
            }
            for (size_t j = 0; j < input.length(); ++j) {
                if ((input[j] >= '0') && (input[j] <= '9')) {
                    if (j == 0) {
                        jobTime = std::stoi(input.substr(j, input.find(' ', j)));
                        jobGraph[std::make_pair(i + 1, jobTime)] = std::set<size_t>();
                        j = input.find(' ', j);
                    }
                    else {
                        jobGraph[std::make_pair(i + 1, jobTime)].insert(std::stoi(input.substr(j, input.find(' ', j))));
                        j = input.find(' ', j);
                    }
                }
                else if (input[j] == '#') {
                    break;std::getline(file, input);
                }
            }
            std::getline(file, input);
        }

        file.close();
    }
    else {
        printf("No input file");
    }
}

int main()
{
    graph jobGraph;
    ReadJobGraphFromFile(jobGraph, "job_graph_3.txt");

    AnnealingSimulation simulation(jobGraph, 4);

    for (auto &i : jobGraph) {
        std::cout << std::to_string(i.first.first) << ": " << std::to_string(i.first.second) << "; ";
        for (auto &j : i.second) {
            std::cout << std::to_string(j) << ", ";
        }
        std::cout << std::endl;
    }

    // simulation.start();
    // simulation.printBestSchedule();
    // std::cout << "Best energy: " << std::to_string(simulation.getBestEnergy()) << std::endl;

    //simulation.printCurrentSchedule();
    //std::cout << "Best energy: " << std::to_string(simulation.getCurrentEnergy()) << std::endl;
    /*
    Schedule s(jobGraph, 3);
    for (auto &i : s.schedule[0]) {
        std::cout << std::to_string(i.first) << " ";
    }
    std::cout << std::endl;
    */

    AnnealingSimulation answer(jobGraph, 4);
    size_t lastUpdated = 0, numThreads = 8;
    std::vector<AnnealingSimulation> solvers;
    for (size_t i = 0; i < numThreads; i++) {
        solvers.emplace_back(jobGraph, 4);
    }
    while (lastUpdated <= max_iterations) {
        std::vector<std::thread> threadPool;

        for (size_t i = 0; i < numThreads; i++) {
            threadPool.emplace_back([&solvers](size_t i) {solvers[i].start();}, i);
        }
        for (auto& thread : threadPool) {
            thread.join();
        }

        for (auto& solver : solvers) {
            if (answer.getBestEnergy() > solver.getBestEnergy()) {
                lastUpdated = 0;
                answer = solver;
            }
        }

        for (auto& i : solvers) {
            i.updateSchedules(answer);
        }
        lastUpdated++;
    }
    answer.printBestSchedule();
    std::cout << "Best energy: " << std::to_string(answer.getBestEnergy()) << std::endl;

    // valgrid dtdb thread debug

    return 0;
}
