#include "LoadData.h"


using namespace std;

// Funkcja do wczytywania danych ATSP z pliku TSPLIB
ATSPData loadATSPData(const string& filename)
{
    ATSPData atspData;
    ifstream file(filename);

    if (!file.is_open())
    {
        throw runtime_error("Nie mozna otworzyc pliku: " + filename);
    }

    string line;
    bool isDistanceSection = false;

    while (getline(file, line))
    {
        istringstream iss(line);
        string token;
        iss >> token;

        // Parsuj sekcje TSPLIB
        if (token == "DIMENSION:")
        {
            iss >> atspData.dimension;
            atspData.distances.resize(atspData.dimension, vector<int>(atspData.dimension, 0));
        }
        else if (token == "EDGE_WEIGHT_SECTION")
        {
            isDistanceSection = true;
            break;
        }
    }

    if (isDistanceSection)
    {
        vector<int> allDistances;
        while (getline(file, line))
        {
            istringstream iss(line);
            int value;
            while (iss >> value)
            {
                allDistances.push_back(value);
            }
        }

        // SprawdŸ, czy liczba wartoœci zgadza siê z wymiarem macierzy
        if (allDistances.size() != static_cast<size_t>(atspData.dimension * atspData.dimension))
        {
            throw runtime_error("Nieprawidlowa wartosc w sekcji EDGE_WEIGHT_SECTION");
        }

        // Przepisz dane do macierzy 2D
        for (int i = 0; i < atspData.dimension; ++i)
        {
            for (int j = 0; j < atspData.dimension; ++j)
            {
                atspData.distances[i][j] = allDistances[i * atspData.dimension + j];
            }
        }
    }

    file.close();
    return atspData;
}

// Funkcja pomocnicza do wyœwietlania danych ATSP
void printATSPData(const ATSPData& data)
{
    cout << "DIMENSION: " << data.dimension << endl;
    cout << "DISTANCES: " << endl;
    for (const auto& row : data.distances)
    {
        for (int dist : row)
        {
            cout << dist << " ";
        }
        cout << endl;
    }
}

vector<int> loadPathFromFile(const string& pathFile) 
{
    vector<int> path;
    ifstream file(pathFile);

    if (file.is_open()) 
    {
        int numVertices;
        file >> numVertices;  // Wczytanie liczby wierzcho³ków

        // Wczytanie œcie¿ki
        path.resize(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            file >> path[i];
        }
        file.close();
    }
    else 
    {
        cerr << "Nie udalo sie otworzyc pliku ze sciezka" << endl;
    }

    return path;
}

int calculatePathCostFromFile(const ATSPData& data, const vector<int>& path)
{
    int totalCost = 0;
    for (size_t i = 0; i < path.size()-1; ++i) 
    {
        totalCost += data.distances[path[i]][path[i + 1]];
    }
    totalCost += data.distances[path[path.size()-1]][path[0]];
    return totalCost;
}
