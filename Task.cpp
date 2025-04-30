#include "Task.h"

// Initialize static weights with default values
double Task::timeUrgencyWeight = 0.25;
double Task::riskFactorWeight = 0.2;
double Task::impactFactorWeight = 0.2;
double Task::timeSensitiveWeight = 0.15;
double Task::taskTypeWeight = 0.1;
double Task::taskAgeWeight = 0.1;

Task::Task(int id, double x, double y,
           double timeLeft, std::string taskType,
           std::string area, std::string population,
           int ageOfTaskInMinutes)
    : id(id), x(x), y(y), timeLeft(timeLeft),
      taskType(taskType), area(area), population(population),
      ageOfTaskInMinutes(ageOfTaskInMinutes) {}

int Task::getId() const { return id; }
double Task::getX() const { return x; }
double Task::getY() const { return y; }

double Task::calculatePriority() const {
    double timeUrgency = 0.0, riskFactor = 0.0, impactFactor = 0.0;
    double timeSensitive = 0.0, taskTypeScore = 0.0, taskAgeFactor = 0.0;

    // 1. Time-Based Urgency
    if (timeLeft <= 0) timeUrgency = 1.0;
    else if (timeLeft <= 5) timeUrgency = 0.95;
    else if (timeLeft <= 10) timeUrgency = 0.85;
    else if (timeLeft <= 20) timeUrgency = 0.7;
    else if (timeLeft <= 30) timeUrgency = 0.6;
    else if (timeLeft <= 45) timeUrgency = 0.5;
    else if (timeLeft <= 60) timeUrgency = 0.3;
    else timeUrgency = 0.1;

    // 2. Risk Factor
    if (taskType == "medical") riskFactor = 1.0;
    else if (taskType == "rescue") riskFactor = 0.95;
    else if (taskType == "explosives_disposal") riskFactor = 0.9;
    else if (taskType == "firefighting") riskFactor = 0.85;
    else if (taskType == "fuel_delivery") riskFactor = 0.7;
    else if (taskType == "surveillance") riskFactor = 0.5;
    else if (taskType == "routine_check") riskFactor = 0.3;
    else riskFactor = 0.1;

    // 3. Impact Factor
    if (area == "warzone" && population == "high") impactFactor = 1.0;
    else if (area == "warzone" && population == "low") impactFactor = 0.9;
    else if (area == "hospital_zone") impactFactor = 0.95;
    else if (area == "city" && population == "high") impactFactor = 0.8;
    else if (area == "rural" && population == "low") impactFactor = 0.5;
    else if (area == "open_field") impactFactor = 0.2;
    else impactFactor = 0.1;

    // 4. Time-Sensitive Score
    if (taskType == "emergency_supply") timeSensitive = 1.0;
    else if (taskType == "organ_transport") timeSensitive = 0.95;
    else if (taskType == "evacuation") timeSensitive = 0.9;
    else if (taskType == "food_delivery") timeSensitive = 0.7;
    else if (taskType == "logistics") timeSensitive = 0.6;
    else if (taskType == "environment_monitoring") timeSensitive = 0.4;
    else if (taskType == "mapping") timeSensitive = 0.2;
    else timeSensitive = 0.1;

    // 5. Task Type Score
    if (taskType == "medical") taskTypeScore = 1.0;
    else if (taskType == "rescue") taskTypeScore = 0.95;
    else if (taskType == "surveillance") taskTypeScore = 0.7;
    else if (taskType == "logistics") taskTypeScore = 0.6;
    else if (taskType == "communication_relay") taskTypeScore = 0.5;
    else if (taskType == "battery_delivery") taskTypeScore = 0.3;
    else taskTypeScore = 0.1;

    // 6. Task Age Factor
    if (ageOfTaskInMinutes >= 60) taskAgeFactor = 1.0;
    else if (ageOfTaskInMinutes >= 45) taskAgeFactor = 0.8;
    else if (ageOfTaskInMinutes >= 30) taskAgeFactor = 0.6;
    else if (ageOfTaskInMinutes >= 15) taskAgeFactor = 0.4;
    else taskAgeFactor = 0.2;

    // Final Weighted Priority Score
    return (timeUrgencyWeight * timeUrgency) +
           (riskFactorWeight * riskFactor) +
           (impactFactorWeight * impactFactor) +
           (timeSensitiveWeight * timeSensitive) +
           (taskTypeWeight * taskTypeScore) +
           (taskAgeWeight * taskAgeFactor);
}