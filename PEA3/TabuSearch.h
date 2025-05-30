#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include <numeric>
#include <vector>
#include <map>
#include <iostream>
#include <random>
#include <algorithm>
#include <windows.h>
#include <iomanip>
#include <chrono>

#include "LoadData.h"

class TabuSearch 
{
public:
    TabuSearch(ATSPData* inputData): data(inputData), finalCost(INT_MAX) {} 

    void beginTabuSearch(ATSPData atspData, int lifetime, int tenure, int neighborSelection, int TS_maxNoImprovement);
    static void showPath(std::vector<unsigned int> path);
    int getFinalCost() const; 
    std::vector<unsigned int> getBestPath() const;

private:
    ATSPData* data;                                   // Przechowywanie wskaŸnika do danych ATSP
    std::vector<unsigned int> globalPath;             // Globalna œcie¿ka 
    std::vector<std::vector<unsigned int>> tabuList;  // Lista tabu
    int finalCost;                                    // Koszt koñcowy 

    void mainLoop(ATSPData atspData, int lifetime, int tenure, int neighborSelection, int TS_maxNoImprovement);
    void generatePath(ATSPData atspData);
    int calculateCost(ATSPData atspData, std::vector<unsigned int> path);
    void decrementTabuList();
    bool checkAspirationCriteria(int i, int j, const std::vector<unsigned int>* path);
    void diversification(ATSPData atspData);
    void findBestNeighbor(ATSPData atspData, std::vector<unsigned int>* path, int *localCost, int tenure, int neighborSelection);
    int swapNeighborsDelta(ATSPData& atspData, std::vector<unsigned int>* path, int i, int j);
    int insertNeighborsDelta(ATSPData& atspData, std::vector<unsigned int>* path, int i, int j);
    int reverseNeighborsDelta(ATSPData& atspData, std::vector<unsigned int>* path, int i, int j);
    void swapNeighbors(std::vector<unsigned int>* path, int i, int j);
    void insertNeighbors(std::vector<unsigned int>* path, int i, int j);
    void reverseNeighbors(std::vector<unsigned int>* path, int i, int j);
};

#endif // TABU_SEARCH_H
