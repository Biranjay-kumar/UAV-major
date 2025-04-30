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
    // Create a priority list of tasks based on calculated priorities
    std::vector<std::pair<int, Task>> prioritizedTasks;
    for (const Task &task : tasks)
    {
        prioritizedTasks.emplace_back(task.calculatePriority(), task);
    }

    std::sort(prioritizedTasks.begin(), prioritizedTasks.end(),
              [](const std::pair<int, Task>& a, const std::pair<int, Task>& b) {
                  return a.first > b.first;
              });

    // Simulate swarm behavior: Assign tasks based on pheromone levels
    for (int i = 0; i < std::min((int)uavs.size(), (int)prioritizedTasks.size()); ++i)
    {
        // Calculate probabilities based on pheromone and distance
        double totalPheromone = 0.0;
        std::vector<double> taskProbabilities;

        // Compute the probability of each task based on pheromone and inverse distance
        for (const auto &taskPair : prioritizedTasks)
        {
            const Task &task = taskPair.second;
            double dist = distance(uavs[i], task);
            double pheromoneStrength = pheromones[uavs[i].getId()][task.getId()];
            double probability = std::pow(pheromoneStrength, ALPHA) * std::pow(1.0 / dist, BETA); // Balance of pheromone and heuristic (distance)
            taskProbabilities.push_back(probability);
            totalPheromone += probability;
        }

        // Select task based on probability distribution
        double randProb = static_cast<double>(rand()) / RAND_MAX * totalPheromone;
        double cumulativeProb = 0.0;
        Task selectedTask = prioritizedTasks[0].second;  // Default fallback in case loop fails

        for (int j = 0; j < taskProbabilities.size(); ++j)
        {
            cumulativeProb += taskProbabilities[j];
            if (cumulativeProb >= randProb)
            {
                selectedTask = prioritizedTasks[j].second;
                break;
            }
        }


        // Assign the selected task to the UAV
        uavs[i].assignTask(selectedTask);
        // std::cout << "Assigned Task ID " << selectedTask.getId()
        //           << " to UAV ID " << uavs[i].getId() << std::endl;

        // Update pheromone levels based on task completion (assuming completion quality or time can affect it)
        double completionQuality = 1.0 / distance(uavs[i], selectedTask);  // Use distance as a simple proxy for quality
        pheromones[uavs[i].getId()][selectedTask.getId()] = (1.0 - EVAPORATION) * pheromones[uavs[i].getId()][selectedTask.getId()] + EVAPORATION * completionQuality;
    }
}

