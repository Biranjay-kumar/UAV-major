

// Scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "UAV.h"
#include "Task.h"
#include <vector>

class Scheduler
{
private:
    int numUAVs;
    int numTasks;
    std::vector<std::vector<double>> pheromones;

    double distance(const UAV &uav, const Task &task);

public:
    Scheduler(int numUAVs, int numTasks);

    void assignTasks(std::vector<UAV> &uavs, std::vector<Task> &tasks);
};

#endif
