#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <limits>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "LoadData.h"

class GeneticAlgorithm
{
private:
    ATSPData* data;                                 // ATSP problem data
    std::vector<unsigned int> globalPath;           // Global best path
    int finalCost;                                  // Final cost
    std::vector<std::vector<unsigned int>> populacja; // Current population
    std::vector<int> costs;                         // Costs of individuals in the population

    void initializePopulation(ATSPData atspData, int wielkoscPopulacji); // Initialize the population
    void evolve(ATSPData atspData, int czasGraniczny, int wielkoscPopulacji, int rodzajKrzyzowania, int rodzajMutacji, double wspolczynnikKrzyzowania, double wspolczynnikMutacji); // Evolve the population
    double calculateRelativeError(double expectedValue);
    int calculateCost(ATSPData atspData, const std::vector<unsigned int>& path);            // Calculate the cost of a path
    void applyElitismAndNiching(int numElites, int numNiches, std::vector<std::vector<unsigned int>>& nowaPopulacja, ATSPData atspData);
    //int similarity(const std::vector<unsigned int>& path1, const std::vector<unsigned int>& path2);
    //void applyClearing(std::vector<std::vector<unsigned int>>& nowaPopulacja, double clearingThreshold);
    //void applyCrowding(std::vector<std::vector<unsigned int>>& nowaPopulacja, std::vector<std::vector<unsigned int>>& staraPopulacja, double crowdingThreshold);
    std::vector<unsigned int> tournamentSelection(ATSPData atspData, int wielkoscPopulacji, int tournamentSize); // Tournament selection
    std::vector<unsigned int> orderedCrossover(const std::vector<unsigned int>& parent1, const std::vector<unsigned int>& parent2); // Ordered crossover
    std::vector<unsigned int> partiallyMappedCrossover(const std::vector<unsigned int>& parent1, const std::vector<unsigned int>& parent2); // Partially mapped crossover
    void swapMutation(std::vector<unsigned int>& path);                 // Swap mutation
    void insertMutation(std::vector<unsigned int>& path);               // Insert mutation
   
    
    void showPath(const std::vector<unsigned int>& path);                                  // Display a path

public:
    // Constructor
    GeneticAlgorithm(ATSPData* inputData) : data(inputData), finalCost(INT_MAX) {}

    // Main entry point for the algorithm
    void beginGenetic(ATSPData atspData, int czasGraniczny, int wielkoscPopulacji, int rodzajKrzyzowania, int rodzajMutacji, double wspolczynnikKrzyzowania, double wspolczynnikMutacji);

    // Getter for the final cost
    int getFinalCost() const;

    // Getter for the best path
    std::vector<unsigned int> getBestPath() const;
};

#endif // GENETIC_ALGORITHM_H
