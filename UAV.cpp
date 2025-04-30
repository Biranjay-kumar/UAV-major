#include "UAV.h"
#include <cmath>
#include <iostream>
#include <chrono>

// Private implementation structure
struct UAV::TaskData {
    Task task;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    double timeLimit; // in seconds
    
    TaskData(const Task& t) : 
        task(t.getId(), t.getX(), t.getY(), t.getTimeLeft(), "", "", "", 0),
        startTime(std::chrono::steady_clock::now()),
        timeLimit(t.getTimeLeft() * 60) {}  // Changed to use getTimeLeft()
};

UAV::UAV(int id, double x, double y, double fuelLevel, double speed) :
    id(id), x(x), y(y), fuelLevel(fuelLevel), speed(speed), available(true),
    currentTask(nullptr) {}

UAV::~UAV() {
    delete currentTask;
}

int UAV::getId() const { return id; }
double UAV::getX() const { return x; }
double UAV::getY() const { return y; }
double UAV::getFuelLevel() const { return fuelLevel; }
bool UAV::isAvailable() const { return available; }

void UAV::assignTask(const Task &task) {
    double dx = task.getX() - x;
    double dy = task.getY() - y;
    double dist = std::sqrt(dx * dx + dy * dy);
    double fuelNeeded = dist * 2;
    
    // Calculate travel time (distance / speed)
    double travelTime = dist / speed;
    
    if (fuelLevel >= fuelNeeded) {
        fuelLevel -= fuelNeeded;
        x = task.getX();
        y = task.getY();
        available = false;
        
        if (currentTask) delete currentTask;
        currentTask = new TaskData(task);

        std::cout << "UAV " << id << " assigned Task " << task.getId()
                  << ", Time Left: " << task.getTimeLeft() << " minutes\n"
                  << "Distance: " << dist << " units\n"
                  << "Travel Time: " << travelTime << " minutes to reach destination\n";
    } else {
        std::cout << "UAV " << id << " doesn't have enough fuel for Task " 
                  << task.getId() << "\n";
    }
}
void UAV::update() {
    if (!available && currentTask) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - currentTask->startTime).count();
        
        double remainingTime = currentTask->timeLimit - elapsed;
        
        if (remainingTime <= 0) {
            std::cout << "UAV " << id << " completed Task " 
                      << currentTask->task.getId() << "\n";
            available = true;
            delete currentTask;
            currentTask = nullptr;
        } else {
            std::cout << "UAV " << id << " working on Task " 
                      << currentTask->task.getId()
                      << ", Time Left: " << (remainingTime / 60) << " minutes\n";
        }
    }
}

void UAV::refuel() {
    fuelLevel = 100.0;
    std::cout << "UAV " << id << " refueled to 100%\n";
}