#include "GeneticAlgorithm.h"
#include <set>

using namespace std;
using namespace chrono;

void GeneticAlgorithm::beginGenetic(ATSPData atspData, int czasGraniczny, int wielkoscPopulacji, int rodzajKrzyzowania, int rodzajMutacji, double wspolczynnikKrzyzowania, double wspolczynnikMutacji)
{
    // Prepare random generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Initialize population
    initializePopulation(atspData, wielkoscPopulacji);

    // Start evolution
    evolve(atspData, czasGraniczny, wielkoscPopulacji, rodzajKrzyzowania, rodzajMutacji, wspolczynnikKrzyzowania, wspolczynnikMutacji);
}

// Generate the initial population
void GeneticAlgorithm::initializePopulation(ATSPData atspData, int wielkoscPopulacji)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    vector<unsigned int> basePath(atspData.dimension);
    iota(basePath.begin(), basePath.end(), 0); // Fill with 0, 1, ..., n-1

    populacja.clear();
    costs.clear();

    for (int i = 0; i < wielkoscPopulacji; ++i)
    {
        shuffle(basePath.begin(), basePath.end(),gen); // Shuffle the base path
        populacja.push_back(basePath);                 // Add to the population
        costs.push_back(calculateCost(atspData, basePath)); // Calculate and store the cost
    }

    // Update globalPath with the best solution in the initial population
    auto minCostIt = min_element(costs.begin(), costs.end());
    globalPath = populacja[minCostIt - costs.begin()];
    finalCost = *minCostIt;
    //showPath(globalPath);
}

// Calculate the cost of an individual path
int GeneticAlgorithm::calculateCost(ATSPData atspData, const vector<unsigned int>& path)
{
    int cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        cost += atspData.distances[path[i]][path[i + 1]];
    }
    cost += atspData.distances[path.back()][path.front()];
    return cost;
}

// Elitism: Retain the best individuals
void GeneticAlgorithm::applyElitismAndNiching(int numElites, int numNiches, vector<vector<unsigned int>>& nowaPopulacja, ATSPData atspData)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    // ZnajdŸ indeksy najlepszych osobników
    vector<size_t> indices(populacja.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](size_t a, size_t b) 
        {
        return costs[a] < costs[b];
        });

    // Dodaj elity do nowej populacji
    for (int i = 0; i < numElites && i < populacja.size(); ++i)
    {
        nowaPopulacja.push_back(populacja[indices[i]]);
    }

    if (populacja.size() > numElites)
    {
        vector<size_t> rejectedIndices(populacja.size() - numElites);
        iota(rejectedIndices.begin(), rejectedIndices.end(), numElites);

        // Losowo wybierz osobniki z pozosta³ych (niching)
        shuffle(rejectedIndices.begin(), rejectedIndices.end(), gen);
        for (int i = 0; i < numNiches && i < rejectedIndices.size(); ++i)
        {
            nowaPopulacja.push_back(populacja[rejectedIndices[i]]);
        }
    }
}

vector<unsigned int> GeneticAlgorithm::tournamentSelection(ATSPData atspData, int wielkoscPopulacji, int tournamentSize)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    // Sprawdzenie poprawnoœci tournamentSize
    if (tournamentSize <= 0 || tournamentSize > wielkoscPopulacji)
    {
        tournamentSize = std::min(3, wielkoscPopulacji);
    }

    std::uniform_int_distribution<> dist(0, wielkoscPopulacji - 1);

    // Set przechowuj¹cy unikalne indeksy
    std::unordered_set<int> selectedIndices;

    // Wybór pierwszego osobnika
    int bestIndex = dist(gen);
    int bestCost = calculateCost(atspData, populacja[bestIndex]);
    selectedIndices.insert(bestIndex);

    for (int i = 1; i < tournamentSize; ++i) {
        int currentIndex;

        // Losowanie unikalnego osobnika
        do {
            currentIndex = dist(gen);
        } while (selectedIndices.find(currentIndex) != selectedIndices.end());

        selectedIndices.insert(currentIndex);
        int currentCost = calculateCost(atspData, populacja[currentIndex]);

        // Sprawdzanie, czy nowy osobnik jest lepszy
        if (currentCost < bestCost) {
            bestIndex = currentIndex;
            bestCost = currentCost;
        }
    }
    return populacja[bestIndex];
}

// Operator krzy¿owania (Ordered Crossover - OX)
vector<unsigned int> GeneticAlgorithm::orderedCrossover(const vector<unsigned int>& parent1, const vector<unsigned int>& parent2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    size_t n = parent1.size();
    vector<unsigned int> child(n, std::numeric_limits<unsigned int>::max());
    vector<bool> used(n, false);

    uniform_int_distribution<int> dist(0, n - 1);
    int start = dist(gen);
    int end = dist(gen);
    if (start > end) swap(start, end);

    // Przepisanie segmentu z parent1
    for (int i = start; i <= end; ++i)
    {
        child[i] = parent1[i];
        used[parent1[i]] = true;
    }

    // Wype³nienie pozosta³ych miast z parent2
    size_t index = 0;
    for (int i = 0; i < n; ++i)
    {
        if (child[i] == std::numeric_limits<unsigned int>::max())
        {
            while (used[parent2[index]])
            {
                ++index;
            }
            child[i] = parent2[index];
            used[parent2[index]] = true;
        }
    }
    return child;
}

vector<unsigned int> GeneticAlgorithm::partiallyMappedCrossover(const vector<unsigned int>& parent1, const vector<unsigned int>& parent2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    size_t n = parent1.size();
    vector<unsigned int> child(n, std::numeric_limits<unsigned int>::max());

    // Losowanie segmentu crossover
    std::uniform_int_distribution<int> dist(0, n - 1);
    int start = dist(gen);
    int end = dist(gen);
    if (start > end) std::swap(start, end);

    // Przepisanie segmentu z parent1
    std::set<unsigned int> used_vertices;
    for (int i = start; i <= end; ++i)
    {
        child[i] = parent1[i];
        used_vertices.insert(parent1[i]); // Oznacz wierzcho³ek jako u¿yty
    }

    // Mapowanie pozosta³ych wartoœci z parent2 do child
    for (int i = 0; i < n; ++i)
    {
        if (i >= start && i <= end)
            continue; // Pomiñ segment ju¿ wype³niony z parent1

        unsigned int value = parent2[i];

        // ZnajdŸ odpowiadaj¹c¹ wartoœæ w parent1, jeœli wyst¹pi konflikt
        while (used_vertices.count(value) > 0)
        {
            auto it = std::find(parent2.begin(), parent2.end(), value);
            if (it == parent2.end())
                break;

            int pos = std::distance(parent2.begin(), it);
            value = parent1[pos];
        }

        // Dodaj wartoœæ do dziecka i oznacz jako u¿yty
        child[i] = value;
        used_vertices.insert(value);
    }

    // SprawdŸ, czy wynikowa œcie¿ka jest poprawnym cyklem Hamiltona
    if (used_vertices.size() != n || std::any_of(child.begin(), child.end(), [](unsigned int v) 
        {
        return v == std::numeric_limits<unsigned int>::max();
        }))
    {
        std::cerr << "B³¹d: Wygenerowana œcie¿ka nie jest poprawna!" << std::endl;
        std::cerr << "Parent1: ";
        for (const auto& v : parent1)
        {
            std::cout << v << " ";
        }
        std::cout << std::endl;

        std::cerr << "Parent2: ";
        for (const auto& v : parent2)
        {
            std::cout << v << " ";
        }
        std::cout << std::endl;

        std::cerr << "Child: ";
        for (const auto& v : child)
        {
            std::cout << v << " ";
        }
        std::cout << std::endl;

        return {}; // Zwróæ pusty wektor w przypadku b³êdu
    }

    return child;
}


// Mutacja typu swap
void GeneticAlgorithm::swapMutation(vector<unsigned int>& path)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, path.size() - 2); // Restrict mutation to inner nodes
    int i = dist(gen);
    int j = dist(gen);
    swap(path[i], path[j]);
}

// Mutacja typu insert
void GeneticAlgorithm::insertMutation(vector<unsigned int>& path)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, path.size() - 2); // Restrict mutation to inner nodes
    int i = dist(gen);
    int j = dist(gen);
    unsigned int city = path[i];
    path.erase(path.begin() + i);
    path.insert(path.begin() + j, city);
}

// Evolution process
void GeneticAlgorithm::evolve(ATSPData atspData, int czasGraniczny, int wielkoscPopulacji, int rodzajKrzyzowania, int rodzajMutacji, double wspolczynnikKrzyzowania, double wspolczynnikMutacji)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    auto startTime = chrono::steady_clock::now();
    auto lastTime = chrono::steady_clock::now();

    while (duration_cast<seconds>(high_resolution_clock::now() - startTime).count() < czasGraniczny)
    {
        std::vector<std::vector<unsigned int>> nowaPopulacja;
        nowaPopulacja.reserve(wielkoscPopulacji);
        costs.clear();

        int numElites = std::min((int)(wielkoscPopulacji * 0.05), (int)populacja.size());
        int numNiches = std::min((int)(wielkoscPopulacji * 0.10), (int)populacja.size() - numElites);

        applyElitismAndNiching(numElites, numNiches, nowaPopulacja, atspData);

        // Generate new individuals
        while (nowaPopulacja.size() < wielkoscPopulacji)
        {
            // Parent selection
            vector<unsigned int> parent1 = tournamentSelection(atspData, wielkoscPopulacji,3);
            vector<unsigned int> parent2 = tournamentSelection(atspData, wielkoscPopulacji,3);

            // Crossover
            vector<unsigned int> child;
            if (wspolczynnikKrzyzowania > uniform_real_distribution<double>(0.0, 1.0)(gen))
            {
                if (rodzajKrzyzowania == 1)
                {
                    child = orderedCrossover(parent1, parent2);         
                }
                else if (rodzajKrzyzowania == 2)
                {
                    child = partiallyMappedCrossover(parent1, parent2);
                }
            }
            else
            {
                child = parent1;
            }

            // Mutation
            if (wspolczynnikMutacji > uniform_real_distribution<double>(0.0, 1.0)(gen))
            {
                if (rodzajMutacji == 1)
                {
                    swapMutation(child);
                }
                else if (rodzajMutacji == 2)
                {
                    insertMutation(child);
                }
            }

            nowaPopulacja.push_back(child);
        }     
        // Update population and costs
        populacja = nowaPopulacja;
        //costs.clear();
        for (const auto& path : populacja)
        {
            costs.push_back(calculateCost(atspData, path));
        }

        // Update global best solution
        auto minCostIt = min_element(costs.begin(), costs.end());
        if (*minCostIt < finalCost)
        {
            finalCost = *minCostIt;
            globalPath = populacja[minCostIt - costs.begin()];
        }
        
        
        // Calculate and display the relative error every 1 second
        auto currentTime = chrono::steady_clock::now();
        if (duration_cast<seconds>(currentTime - lastTime).count() >= 1)
        {
            double relativeError = calculateRelativeError(2755.0);
            cout << relativeError << endl;
            lastTime = currentTime; 
        }
    }
}

double GeneticAlgorithm::calculateRelativeError(double expectedValue)
{
    if (costs.empty()) return 0.0;

    double averageCost = accumulate(costs.begin(), costs.end(), 0.0) / costs.size();
    return abs(averageCost - expectedValue) / expectedValue * 100.0; // Return percentage error
}

// Display the path
void GeneticAlgorithm::showPath(const vector<unsigned int>& path)
{
    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        cout << path[i] << " -> ";
    }
    cout << path.back() << endl;
}

// Getter for the final cost
int GeneticAlgorithm::getFinalCost() const
{
    return finalCost;
}

// Getter for the best path
vector<unsigned int> GeneticAlgorithm::getBestPath() const
{
    return globalPath;
}
