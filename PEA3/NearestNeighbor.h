#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <climits>
#include <cstring>

#include "LoadData.h"


class NearestNeighbor
{
public:
    NearestNeighbor(ATSPData *atspData) : finalCost(INT_MAX) {} 

    void start(ATSPData atspData); 
    int getFinalCost() const;
    std::vector<unsigned> getBestPath() const;

private:
    std::vector<unsigned> globalPath;   // Globalna œcie¿ka 
    int finalCost;                      // Koszt koñcowy
};

#endif // NEAREST_NEIGHBOR_H
