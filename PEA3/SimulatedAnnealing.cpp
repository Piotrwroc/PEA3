#include "SimulatedAnnealing.h"

using namespace std;
using namespace std;
using namespace chrono;

void SimulatedAnnealing::beginSimulatedAnnealing(ATSPData atspData, double const alfa, int lifetime)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    generatePath(atspData);

    finalCost = calculateCost(atspData, globalPath);
    currentTemperature = initialTemperature(alfa);


    mainLoop(atspData, alfa, lifetime);

}

void SimulatedAnnealing::generatePath(ATSPData atspData)
{

    // Generowanie listy dostêpnych wierzcho³ków
    vector<unsigned int> availableVertices(data->dimension); 
    iota(availableVertices.begin(), availableVertices.end(), 0); // Wype³nienie 0...n-1

    // Przygotowanie generatora losowego
    std::random_device rd;
    std::mt19937 gen(rd());
    globalPath.clear();

    shuffle(availableVertices.begin(), availableVertices.end(), gen); // Tasowanie wierzcho³ków
    globalPath = availableVertices;                                         // Dodanie losowej permutacji jako œcie¿ki
    globalPath.push_back(globalPath.front());                                    // Dodanie powrotu do pierwszego wierzcho³ka, aby utworzyæ cykl Hamiltona
    //showPath(globalPath);
}

double SimulatedAnnealing::initialTemperature(double const alfa)
{
    return finalCost * alfa;
}

void SimulatedAnnealing::mainLoop(ATSPData atspData, const double alfa, int lifetime)
{
    int expectedValue = 1163;
    auto startTime = high_resolution_clock::now();
    auto bestSolutionTime = startTime;

    std::random_device rd;
    std::default_random_engine g(rd());

    vector<unsigned> path = globalPath;
    int localMinCost = calculateCost(atspData, path);
    int localCost;
    pair<int, int> coordinates;

    std::uniform_int_distribution<int> swapIterator(1, data->dimension - 1);
    std::uniform_real_distribution<> generateProbability(0.0, 1.0);

    vector<unsigned int> bestGlobalPath = path;
    int bestFinalCost = localMinCost;

    int noImprovementCount = 0; // Liczba iteracji bez poprawy rozwi¹zania
    int maxNoImprovement = 1000; // Maksymalna liczba iteracji bez poprawy


    while (duration_cast<seconds>(high_resolution_clock::now() - startTime).count() < lifetime) // lifetime w sekundach
    {
        for (int i = 0; i < 1000; i++)
        {
            localCost = localMinCost;
            do
            {
                coordinates.first = swapIterator(g);
                coordinates.second = swapIterator(g);
            } while (coordinates.first == coordinates.second);

            localCost += generateRandomNeighbour(atspData, path, coordinates);

            if (localCost < finalCost)
            {
                //cout << "Nastapila poprawa " << localCost << endl;
                finalCost = localCost;
                globalPath = path;
                swap(globalPath.at(coordinates.first), globalPath.at(coordinates.second));
                noImprovementCount = 0; // Resetujemy licznik poprawy
            }

            int delta = localCost - localMinCost;

            // Intensyfikacja i akceptacja rozwi¹zania
            if (delta < 0 || exp((localMinCost - localCost) / currentTemperature) > generateProbability(g))
            {
                localMinCost = localCost;
                swap(path.at(coordinates.first), path.at(coordinates.second));
            }

            // Zapamiêtanie najlepszej œcie¿ki i kosztu w ca³ym przebiegu
            if (localCost < bestFinalCost)
            {
                bestFinalCost = localCost;
                bestGlobalPath = path;
                bestSolutionTime = high_resolution_clock::now();
                std::cout 
                    << duration_cast<seconds>(high_resolution_clock::now() - startTime).count() << " - " << float(bestFinalCost - expectedValue) / expectedValue * 100.0 << endl;
            }
        }
        currentTemperature *= alfa;
        if (currentTemperature < 1e-10)
        {
            std::cout << "Temperatura za niska!" << endl;
            break;
        }
    }
    // Przypisanie najlepszego rozwi¹zania
    finalCost = bestFinalCost;
    globalPath = bestGlobalPath;

    // Wypisanie najlepszych wyników
    std::cout << "Najlepszy czas w : "
        << duration_cast<seconds>(bestSolutionTime - startTime).count()
        << " sekund od uruchomienia" << endl;
    std::cout << (char)7 << endl;
}

int SimulatedAnnealing::generateRandomNeighbour(ATSPData atspData, vector<unsigned int>& path2, pair<int, int>& c)
{
    if (c.first == c.second)
        return 0; // Ignoruj, jeœli wybrano te same indeksy

    if (c.first > c.second)
        return swapNeighbors(atspData, path2, c.second, c.first);
    else
        return swapNeighbors(atspData, path2, c.first, c.second);
}

bool SimulatedAnnealing::checkToChangeWorstSolution(int const delta, double probability)
{
    if (delta == 0)
    {
        return false;
    }

    return probability < coolingFunction(delta);
}

void SimulatedAnnealing::calculateTemperature(double const alfa)
{
    currentTemperature *= alfa;
}

double SimulatedAnnealing::coolingFunction(int const delta)
{
    return exp(-delta / currentTemperature);
}

int SimulatedAnnealing::swapNeighbors(ATSPData& atspData, vector<unsigned int> path, int i, int j)
{
    int subtractOldEdges = 0;
    int addNewEdges = 0;

    if (j - i == 1)
    {
        subtractOldEdges += data->distances[path.at(i - 1)][path.at(i)];
        subtractOldEdges += data->distances[path.at(i)][path.at(j)];
        subtractOldEdges += data->distances[path.at(j)][path.at(j + 1)];

        addNewEdges += data->distances[path.at(i - 1)][path.at(j)];
        addNewEdges += data->distances[path.at(j)][path.at(i)];
        addNewEdges += data->distances[path.at(i)][path.at(j + 1)];
    }
    else
    {
        subtractOldEdges += data->distances[path.at(i - 1)][path.at(i)];
        subtractOldEdges += data->distances[path.at(i)][path.at(i + 1)];
        subtractOldEdges += data->distances[path.at(j - 1)][path.at(j)];
        subtractOldEdges += data->distances[path.at(j)][path.at(j + 1)];

        addNewEdges += data->distances[path.at(i - 1)][path.at(j)];
        addNewEdges += data->distances[path.at(j)][path.at(i + 1)];
        addNewEdges += data->distances[path.at(j - 1)][path.at(i)];
        addNewEdges += data->distances[path.at(i)][path.at(j + 1)];
    }
    swap(path[i], path[j]); // Dodano faktyczn¹ zamianê wierzcho³ków
    return addNewEdges - subtractOldEdges;
}

// Wyœwietlanie œcie¿ki
void SimulatedAnnealing::showPath(vector<unsigned int> path)
{
    for (size_t i = 0; i < path.size(); i++)
    {
        std::cout << path[i];
        if (i < path.size() - 1)
        {
            std::cout << "->";
        }
    }
    std::cout << endl;
}

// Obliczanie kosztu œcie¿ki
int SimulatedAnnealing::calculateCost(ATSPData atspData, vector<unsigned int> path)
{
    int cost = 0;
    for (size_t i = 0; i < path.size() - 1; i++)
    {
        cost += atspData.distances[path[i]][path[i + 1]];
    }
    return cost;
}

int SimulatedAnnealing::getFinalCost() const
{
    return finalCost;
}

vector<unsigned int> SimulatedAnnealing::getBestPath() const
{
    return globalPath;
}

double SimulatedAnnealing::getCurrentTemperature() const
{
    return currentTemperature;
}

double SimulatedAnnealing::getExponentialDecay() const
{
    return exp(-1 / currentTemperature);
}

