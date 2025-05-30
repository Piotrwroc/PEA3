#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "LoadData.h" 

class SimulatedAnnealing 
{
public:
    SimulatedAnnealing(ATSPData* inputData) : data(inputData), finalCost(INT_MAX) {}
    // Funkcje publiczne
    void beginSimulatedAnnealing(ATSPData atspData, double const alfa, int lifetime);
    void showPath(std::vector<unsigned int> path);

    // Gettery
    int getFinalCost() const;
    std::vector<unsigned int> getBestPath() const;
    double getCurrentTemperature() const;
    double getExponentialDecay() const;

private:
    ATSPData* data;                         // Dane problemu ATSP
    std::vector<unsigned int> globalPath;   // Najlepsza œcie¿ka
    int finalCost;                          // Koszt najlepszej œcie¿ki
    double currentTemperature;              // Aktualna temperatura

    // Funkcje pomocnicze
    void generatePath(ATSPData atspData);
    double initialTemperature(double const alfa);
    void mainLoop(ATSPData atspData, const double alfa, int lifetime);
    int generateRandomNeighbour(ATSPData atspData, std::vector<unsigned int>& path2, std::pair<int, int>& c);
    bool checkToChangeWorstSolution(int const delta, double probability);
    void calculateTemperature(double const alfa);
    double coolingFunction(int const delta);
    int swapNeighbors(ATSPData& atspData, std::vector<unsigned int> path, int i, int j);
    int calculateCost(ATSPData atspData, std::vector<unsigned int> path);
};

#endif // SIMULATED_ANNEALING_H
