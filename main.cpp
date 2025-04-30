#include <iostream>
#include <vector>
#include <string>
#include "UAV.h"
#include "Task.h"
#include "SwarmOptimizer.h"

int main() {
    try {
        // Input UAVs
        int numUAVs;
        std::cout << "Enter number of UAVs: ";
        std::cin >> numUAVs;
        if (numUAVs <= 0) throw std::invalid_argument("Number of UAVs must be positive.");

        std::vector<UAV> uavs;
        for (int i = 0; i < numUAVs; ++i) {
            int id; double x, y, fuel, speed;
            std::cout << "\nUAV " << i + 1 << " ID, X, Y, Fuel, Speed: ";
            std::cin >> id >> x >> y >> fuel >> speed;
            if (fuel <= 0 || speed <= 0) throw std::invalid_argument("Fuel/speed must be positive.");
            uavs.emplace_back(id, x, y, fuel, speed);
        }

        // Input Tasks
        int numTasks;
        std::cout << "Enter number of Tasks: ";
        std::cin >> numTasks;
        if (numTasks <= 0) throw std::invalid_argument("Number of tasks must be positive.");

        std::vector<Task> tasks;
        for (int i = 0; i < numTasks; ++i) {
            int id, age; double x, y, timeLeft;
            std::string type, area, pop;
            std::cout << "\nTask " << i + 1 << " ID, X, Y, TimeLeft, Type, Area, Pop, Age: ";
            std::cin >> id >> x >> y >> timeLeft >> type >> area >> pop >> age;
            tasks.emplace_back(id, x, y, timeLeft, type, area, pop, age);
        }

        // Configure PSO
        int swarmSize, maxIters;
        std::cout << "Enter swarm size (default: " << numUAVs * 10 << "): ";
        std::cin >> swarmSize;
        if (swarmSize <= 0) swarmSize = numUAVs * 10;

        std::cout << "Enter max iterations (default: 100): ";
        std::cin >> maxIters;
        if (maxIters <= 0) maxIters = 100;

        // Run PSO
        SwarmOptimizer optimizer(uavs, tasks, swarmSize, maxIters);
        optimizer.run();

        // Print results
        std::cout << "\n=== Optimal Assignment ===\n";
        const auto& bestAssign = optimizer.getBestAssignment();
        for (int t = 0; t < numTasks; ++t) {
            std::cout << "Task " << t << " → UAV " << bestAssign[t] << "\n";
        }
        std::cout << "Total cost: " << optimizer.getBestFitness() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}