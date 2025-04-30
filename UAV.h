#ifndef UAV_H
#define UAV_H

#include "Task.h"
#include <memory>

class UAV
{
private:
    int id;
    double x, y;
    double fuelLevel;
    bool available;
    
    // Private implementation
    struct TaskData;
    TaskData* currentTask;
    double speed;

public:
    // UAV(int id, double x, double y, double fuelLevel);
    UAV(int id, double x, double y, double fuelLevel, double speed);
    ~UAV();

    int getId() const;
    double getX() const;
    double getY() const;
    double getFuelLevel() const;
    bool isAvailable() const;

    double getSpeed() const { return speed; } 

    void assignTask(const Task &task);
    void refuel();
    void update();
};

#endif