#ifndef TASK_H
#define TASK_H

#include <string>

class Task
{
private:
    int id;
    double x, y;
    double timeLeft;
    std::string taskType;
    std::string area;
    std::string population;
    int ageOfTaskInMinutes;
    static double timeUrgencyWeight;
    static double riskFactorWeight;
    static double impactFactorWeight;
    static double timeSensitiveWeight;
    static double taskTypeWeight;
    static double taskAgeWeight;

public:
    Task(int id, double x, double y,
         double timeLeft, std::string taskType,
         std::string area, std::string population,
         int ageOfTaskInMinutes);
         static void setPriorityWeights(double tu, double rf, double imp, 
            double ts, double tt, double ta) {
                timeUrgencyWeight = tu;
                riskFactorWeight = rf;
                impactFactorWeight = imp;
                timeSensitiveWeight = ts;
                taskTypeWeight = tt;
                taskAgeWeight = ta;
            }
    double getTimeLeft() const { return timeLeft; }  
    // double getTimeLeft() const { return timeLeft; }

    int getId() const;
    double getX() const;
    double getY() const;
    double calculatePriority() const;
};

#endif
