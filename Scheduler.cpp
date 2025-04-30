#include "Scheduler.h"
#include <iostream>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>  // <-- Required for std::sort

const double ALPHA = 1.0; // Importance of pheromone
const double BETA = 2.0;  // Importance of heuristic (inverse distance)
const double EVAPORATION = 0.1;
const double INITIAL_PHEROMONE = 1.0;

Scheduler::Scheduler(int numUAVs, int numTasks)
    : numUAVs(numUAVs), numTasks(numTasks)
{
    pheromones = std::vector<std::vector<double>>(numUAVs, std::vector<double>(numTasks, INITIAL_PHEROMONE));
}

double Scheduler::distance(const UAV &uav, const Task &task)
{
    return std::sqrt(std::pow(uav.getX() - task.getX(), 2) + std::pow(uav.getY() - task.getY(), 2));
}

void Scheduler::assignTasks(std::vector<UAV> &uavs, std::vector<Task> &tasks)
{
    std::vector<std::pair<int, Task>> prioritizedTasks;

    for (const Task &task : tasks)
    {
        prioritizedTasks.emplace_back(task.calculatePriority(), task);
    }

    std::sort(prioritizedTasks.begin(), prioritizedTasks.end(),
              [](const std::pair<int, Task>& a, const std::pair<int, Task>& b) {
                  return a.first > b.first;
              });

    for (int i = 0; i < std::min((int)uavs.size(), (int)prioritizedTasks.size()); ++i)
    {
        uavs[i].assignTask(prioritizedTasks[i].second);
        std::cout << "Assigned Task ID " << prioritizedTasks[i].second.getId()
                  << " to UAV ID " << uavs[i].getId() << std::endl;
    }
}
