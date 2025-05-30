// ATSPData.h
#ifndef LOADDATA_H
#define LOADDATA_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


// Struktura do przechowywania danych ATSP
struct ATSPData
{
    int dimension;                            // Liczba wierzcho³ków
    std::vector<std::vector<int>> distances; // Macierz odleg³oœci
};

// Deklaracje funkcji
ATSPData loadATSPData(const std::string& filename);
void printATSPData(const ATSPData& data);
std::vector<int> loadPathFromFile(const std::string& pathFile);
int calculatePathCostFromFile(const ATSPData& data, const std::vector<int>& path);

#endif // LOADDATA_H