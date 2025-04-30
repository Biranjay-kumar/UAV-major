#ifndef SWARM_OPTIMIZER_H
#define SWARM_OPTIMIZER_H

#include <vector>
#include <random>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <numeric>
#include <functional>
#include <stdexcept>
#include <iomanip>
#include "UAV.h"
#include "Task.h"

struct Particle {
    std::vector<int> assignment;    // assignment[t] = UAV-index assigned to task t
    std::vector<double> velocity;   // continuous velocity for discrete update
    std::vector<int> bestAssign;    // pBest - best position this particle has found
    double fitness = std::numeric_limits<double>::max();
    double bestFitness = std::numeric_limits<double>::max(); // pBest fitness
    
    // For visualization/debugging
    void print(int particleId) const {
        std::cout << "Particle " << particleId << ":\n";
        std::cout << "  Current Assignment: ";
        for (int a : assignment) std::cout << a << " ";
        std::cout << "\n  Current Fitness: " << fitness << "\n";
        std::cout << "  pBest Assignment: ";
        for (int a : bestAssign) std::cout << a << " ";
        std::cout << "\n  pBest Fitness: " << bestFitness << "\n\n";
    }
};

class SwarmOptimizer {
public:
    SwarmOptimizer(const std::vector<UAV>& uavs,
                  const std::vector<Task>& tasks,
                  int swarmSize,
                  int maxIters,
                  double inertiaWeight = 0.729,
                  double cognitiveWeight = 1.49445,
                  double socialWeight = 1.49445)
        : uavs(uavs), tasks(tasks),
          numUAVs(uavs.size()), numTasks(tasks.size()),
          swarmSize(swarmSize), maxIters(maxIters),
          w(inertiaWeight), c1(cognitiveWeight), c2(socialWeight),
          gen(std::random_device{}()), 
          dist01(0.0, 1.0),
          uavDist(0, numUAVs - 1)
    {
        validateParameters();
        initializeCostMatrix();
        initializeSwarm();
        
        // Initialize iteration tracking
        iterationFitnessHistory.reserve(maxIters);
        gBestHistory.reserve(maxIters);
    }

    void run() {
        std::cout << "=== Starting Swarm Optimization ===\n";
        std::cout << "Swarm Size: " << swarmSize << "\n";
        std::cout << "Max Iterations: " << maxIters << "\n";
        std::cout << "Inertia Weight (w): " << w << "\n";
        std::cout << "Cognitive Weight (c1): " << c1 << "\n";
        std::cout << "Social Weight (c2): " << c2 << "\n\n";
        
        printInitialState();

        for (int iter = 0; iter < maxIters; ++iter) {
            currentIteration = iter;
            evaluateSwarm();
            updateGlobalBest();
            
            // Store history for analysis
            iterationFitnessHistory.push_back(globalBestFitness);
            gBestHistory.push_back(globalBestAssign);
            
            if (iter % 10 == 0 || iter == maxIters - 1) { 
                printIterationStatus(iter);
            }

            updateSwarm();
        }

        printFinalResults();
        printConvergenceAnalysis();
    }

    const std::vector<int>& getBestAssignment() const {
        return globalBestAssign;
    }

    double getBestFitness() const {
        return globalBestFitness;
    }
    
    const std::vector<double>& getFitnessHistory() const {
        return iterationFitnessHistory;
    }
    
    const std::vector<std::vector<int>>& getGBestHistory() const {
        return gBestHistory;
    }

private:
    // Constants and references
    const std::vector<UAV>& uavs;
    const std::vector<Task>& tasks;
    const int numUAVs;
    const int numTasks;
    const int swarmSize;
    const int maxIters;
    const double w;  // inertia weight
    const double c1; // cognitive weight
    const double c2; // social weight

    // Optimization state
    std::vector<std::vector<double>> cost;
    std::vector<Particle> swarm;
    std::vector<int> globalBestAssign;  // gBest - best position found by entire swarm
    double globalBestFitness = std::numeric_limits<double>::max();
    int currentIteration = 0;
    
    // For tracking convergence and history
    std::vector<double> iterationFitnessHistory;
    std::vector<std::vector<int>> gBestHistory;

    // Random number generation
    std::mt19937 gen;
    std::uniform_real_distribution<> dist01;
    std::uniform_int_distribution<> uavDist;

    void validateParameters() {
        if (numUAVs == 0 || numTasks == 0) {
            throw std::invalid_argument("UAVs and Tasks lists cannot be empty");
        }
        if (swarmSize <= 0 || maxIters <= 0) {
            throw std::invalid_argument("Swarm size and max iterations must be positive");
        }
        if (w < 0 || c1 < 0 || c2 < 0) {
            throw std::invalid_argument("PSO weights cannot be negative");
        }
    }

    void initializeCostMatrix() {
        cost.resize(numUAVs, std::vector<double>(numTasks));
        for (int i = 0; i < numUAVs; ++i) {
            for (int j = 0; j < numTasks; ++j) {
                cost[i][j] = distance(uavs[i], tasks[j]);
            }
        }
    }

    void initializeSwarm() {
        swarm.resize(swarmSize);
        for (int i = 0; i < swarmSize; ++i) {
            auto& p = swarm[i];
            p.assignment.resize(numTasks);
            p.velocity.resize(numTasks);
            p.bestAssign.resize(numTasks);

            // Initialize with random assignments and velocities
            for (int t = 0; t < numTasks; ++t) {
                p.assignment[t] = uavDist(gen);
                p.velocity[t] = dist01(gen) * numUAVs;
            }
            
            // Evaluate and set initial bests
            p.fitness = evaluate(p.assignment);
            p.bestAssign = p.assignment;
            p.bestFitness = p.fitness;

            // Update global best if needed
            if (p.fitness < globalBestFitness) {
                globalBestFitness = p.fitness;
                globalBestAssign = p.assignment;
            }
        }
    }

    void evaluateSwarm() {
        for (int i = 0; i < swarmSize; ++i) {
            auto& p = swarm[i];
            p.fitness = evaluate(p.assignment);
            if (p.fitness < p.bestFitness) {
                p.bestAssign = p.assignment;
                p.bestFitness = p.fitness;
            }
        }
    }

    void updateGlobalBest() {
        for (const auto& p : swarm) {
            if (p.fitness < globalBestFitness) {
                globalBestFitness = p.fitness;
                globalBestAssign = p.assignment;
            }
        }
    }

		void updateSwarm() {
			for (int i = 0; i < swarmSize; ++i) {  
					Particle& p = swarm[i];  
					for (int t = 0; t < numTasks; ++t) { 
							double r1 = dist01(gen);  
							double r2 = dist01(gen); 
							
							// Update velocity using PSO formula
							p.velocity[t] = w * p.velocity[t] 
															+ c1 * r1 * (p.bestAssign[t] - p.assignment[t]) 
															+ c2 * r2 * (globalBestAssign[t] - p.assignment[t]);
							
							// Sigmoid function to convert velocity to probability
							double prob = sigmoid(p.velocity[t]);
							
							// Update assignment based on probability
							if (dist01(gen) < prob) {
									p.assignment[t] = p.bestAssign[t];
							} else {
									p.assignment[t] = globalBestAssign[t];
							}
	
							// Bound check for the assignments to ensure they stay within UAV limits
							if (p.assignment[t] < 0) p.assignment[t] = 0;
							if (p.assignment[t] >= numUAVs) p.assignment[t] = numUAVs - 1;
					}
			}
	
			// Update the distance (cost matrix) after modifying the assignments
			updateDistance();
	}
	
    void printInitialState() const {
        std::cout << "=== Initial Swarm State ===\n";
        std::cout << "Number of UAVs: " << numUAVs << "\n";
        std::cout << "Number of Tasks: " << numTasks << "\n";
        
        std::cout << "\nCost Matrix (UAVs x Tasks):\n";
        for (int i = 0; i < numUAVs; ++i) {
            for (int j = 0; j < numTasks; ++j) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(2) << cost[i][j] << " ";
            }
            std::cout << "\n";
        }
        
        std::cout << "\nInitial Particles:\n";
        for (int i = 0; i < std::min(3, swarmSize); ++i) { 
            swarm[i].print(i);
        }
        if (swarmSize > 3) {
            std::cout << "... and " << (swarmSize - 3) << " more particles\n";
        }
        
        std::cout << "Initial gBest Fitness: " << globalBestFitness << "\n";
        std::cout << "Initial gBest Assignment: ";
        for (int a : globalBestAssign) std::cout << a << " ";
        std::cout << "\n\n";
    }
    
    void printIterationStatus(int iter) const {
        std::cout << "Iteration " << iter + 1 << "/" << maxIters << ":\n";
        std::cout << "  Current gBest Fitness: " << globalBestFitness << "\n";
        std::cout << "  Current gBest Assignment: ";
        for (int a : globalBestAssign) std::cout << a << " ";
        std::cout << "\n";
        
        // Print some particle info
        if (swarmSize <= 5) {
            for (int i = 0; i < swarmSize; ++i) {
                swarm[i].print(i);
            }
        } else {
            std::cout << "  Sample particle fitnesses: ";
            for (int i = 0; i < std::min(5, swarmSize); ++i) {
                std::cout << "P" << i << "=" << swarm[i].fitness << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    void printFinalResults() const {
        std::cout << "\n=== Optimization Results ===\n";
        std::cout << "Best Solution Found at Iteration: " 
                 << std::distance(iterationFitnessHistory.begin(), 
                                 std::min_element(iterationFitnessHistory.begin(), 
                                                 iterationFitnessHistory.end())) + 1 << "\n";
        std::cout << "Final gBest Fitness: " << globalBestFitness << "\n";
        std::cout << "Final Assignment (Task -> UAV):\n";
        for (int t = 0; t < numTasks; ++t) {
            std::cout << "  Task " << t << " -> UAV " << globalBestAssign[t] 
                     << " (Cost: " << cost[globalBestAssign[t]][t] << ")\n";
        }
    }
    
    void printConvergenceAnalysis() const {
        std::cout << "\n=== Convergence Analysis ===\n";
        std::cout << "Fitness Improvement: " << iterationFitnessHistory.front() 
                 << " -> " << iterationFitnessHistory.back() 
                 << " (" << ((iterationFitnessHistory.front() - iterationFitnessHistory.back()) / 
                            iterationFitnessHistory.front() * 100) << "% improvement)\n";
        
        // Find when gBest was last updated
        int lastUpdateIter = 0;
        double minSoFar = iterationFitnessHistory[0];
        for (size_t i = 1; i < iterationFitnessHistory.size(); ++i) {
            if (iterationFitnessHistory[i] < minSoFar) {
                minSoFar = iterationFitnessHistory[i];
                lastUpdateIter = i;
            }
        }
        std::cout << "Last significant improvement at iteration: " << lastUpdateIter + 1 << "\n";
    }

		void updateDistance() {
			for (int i = 0; i < numUAVs; ++i) {
					for (int j = 0; j < numTasks; ++j) {
							cost[i][j] = distance(uavs[i], tasks[j]);
					}
			}
	}
	

    // Helper functions
    double distance(const UAV& u, const Task& t) const {
        return std::hypot(u.getX() - t.getX(), u.getY() - t.getY());
    }

    double evaluate(const std::vector<int>& assign) const {
        double sum = 0.0;
        for (int t = 0; t < numTasks; ++t) {
            sum += cost[assign[t]][t];
        }
        return sum;
    }

    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }
};

#endif // SWARM_OPTIMIZER_H