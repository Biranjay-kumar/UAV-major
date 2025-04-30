#include <iostream>
#include <vector>
#include <string>
#include "UAV.h"
#include "Task.h"
#include "Scheduler.h"

void displayPriorityOptions() {
    std::cout << "\nSelect Priority Calculation Method:\n";
    std::cout << "1. Default Priority (Weighted combination of all factors)\n";
    std::cout << "2. Time-Critical Priority (Focus on time urgency and deadlines)\n";
    std::cout << "3. Risk-Based Priority (Focus on risk factors and task types)\n";
    std::cout << "4. Impact-Based Priority (Focus on population and area impact)\n";
    std::cout << "5. Custom Priority (Enter your own weights)\n";
    std::cout << "Enter your choice (1-5): ";
}

void getCustomWeights(double& timeUrgencyWeight, double& riskFactorWeight, 
                     double& impactFactorWeight, double& timeSensitiveWeight,
                     double& taskTypeWeight, double& taskAgeWeight) {
    std::cout << "\nEnter custom weights for each factor (must sum to 1.0):\n";
    
    std::cout << "Time Urgency Weight (0-1): ";
    std::cin >> timeUrgencyWeight;
    
    std::cout << "Risk Factor Weight (0-1): ";
    std::cin >> riskFactorWeight;
    
    std::cout << "Impact Factor Weight (0-1): ";
    std::cin >> impactFactorWeight;
    
    std::cout << "Time Sensitive Weight (0-1): ";
    std::cin >> timeSensitiveWeight;
    
    std::cout << "Task Type Weight (0-1): ";
    std::cin >> taskTypeWeight;
    
    std::cout << "Task Age Weight (0-1): ";
    std::cin >> taskAgeWeight;
    
    // Validate the sum is 1.0
    double total = timeUrgencyWeight + riskFactorWeight + impactFactorWeight +
                  timeSensitiveWeight + taskTypeWeight + taskAgeWeight;
    
    if (total != 1.0) {
        std::cout << "Warning: Weights don't sum to 1.0. Normalizing weights...\n";
        timeUrgencyWeight /= total;
        riskFactorWeight /= total;
        impactFactorWeight /= total;
        timeSensitiveWeight /= total;
        taskTypeWeight /= total;
        taskAgeWeight /= total;
    }
}

int main()
{
    int numUAVs, numTasks;
    int priorityChoice;

    std::cout << "Enter number of UAVs: ";
    std::cin >> numUAVs;

    std::vector<UAV> uavs;
for (int i = 0; i < numUAVs; ++i)
{
    int id;
    double x, y, fuel, speed;
    
    std::cout << "\nEnter details for UAV " << i + 1 << ":\n";
    std::cout << "ID: "; std::cin >> id;
    std::cout << "Initial X coordinate: "; std::cin >> x;
    std::cout << "Initial Y coordinate: "; std::cin >> y;
    std::cout << "Fuel Level: "; std::cin >> fuel;
    
    do {
        std::cout << "Speed (units per minute, must be > 0): ";
        std::cin >> speed;
        if (speed <= 0) {
            std::cout << "Invalid speed! Must be positive.\n";
        }
    } while (speed <= 0);
    
    uavs.emplace_back(id, x, y, fuel, speed);
}

    std::cout << "\nEnter number of Tasks: ";
    std::cin >> numTasks;

    std::vector<Task> tasks;
    for (int i = 0; i < numTasks; ++i)
    {
        int id, age;
        double x, y, timeLeft;
        std::string taskType, area, population;

        std::cout << "\nEnter details for Task " << i + 1 << ":\n";
        std::cout << "ID: "; std::cin >> id;
        std::cout << "X Y: "; std::cin >> x >> y;
        std::cout << "Time Left (in minutes): "; std::cin >> timeLeft;
        std::cout << "Task Type: "; std::cin >> taskType;
        std::cout << "Area: "; std::cin >> area;
        std::cout << "Population (low/high): "; std::cin >> population;
        std::cout << "Age of Task (in minutes): "; std::cin >> age;

        tasks.emplace_back(id, x, y, timeLeft, taskType, area, population, age);
    }
    for (auto& uav : uavs) {
        uav.update();  // Check for task completion
    }
    // Priority selection
    displayPriorityOptions();
    std::cin >> priorityChoice;

    // Set priority calculation method based on user choice
    switch(priorityChoice) {
        case 1: // Default
            Task::setPriorityWeights(0.25, 0.2, 0.2, 0.15, 0.1, 0.1);
            break;
        case 2: // Time-Critical
            Task::setPriorityWeights(0.5, 0.15, 0.1, 0.15, 0.05, 0.05);
            break;
        case 3: // Risk-Based
            Task::setPriorityWeights(0.1, 0.4, 0.2, 0.1, 0.15, 0.05);
            break;
        case 4: // Impact-Based
            Task::setPriorityWeights(0.15, 0.15, 0.4, 0.1, 0.1, 0.1);
            break;
        case 5: // Custom
            {
                double tu, rf, imp, ts, tt, ta;
                getCustomWeights(tu, rf, imp, ts, tt, ta);
                Task::setPriorityWeights(tu, rf, imp, ts, tt, ta);
            }
            break;
        default:
            std::cout << "Invalid choice. Using default priority calculation.\n";
            Task::setPriorityWeights(0.25, 0.2, 0.2, 0.15, 0.1, 0.1);
    }

    Scheduler scheduler(numUAVs, numTasks);
    scheduler.assignTasks(uavs, tasks);

    return 0;
}