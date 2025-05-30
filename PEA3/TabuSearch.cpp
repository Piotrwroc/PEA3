#include "TabuSearch.h"

using namespace std;
using namespace chrono;

void TabuSearch::beginTabuSearch(ATSPData atspData, int lifetime, int tenure, int neighborSelection, int TS_maxNoImprovement)
{
    generatePath(atspData);

    mainLoop(atspData, lifetime, tenure, neighborSelection, TS_maxNoImprovement);
}

// G³ówna pêtla programu
void TabuSearch::mainLoop(ATSPData atspData, int lifetime, int tenure, int neighborSelection, int TS_maxNoImprovement)
{
    int expectedValue = 1163;
    auto startTime = high_resolution_clock::now();
    auto bestSolutionTime = startTime; 
    vector<unsigned int> path = globalPath;
    int localCost = finalCost;
    int iterCountermax = 1000;
    if (neighborSelection == 1)
    {
        iterCountermax = 2500;
    }
    else if (neighborSelection == 2)
    {
        iterCountermax = 25000;
    }

    bool aspirationCriteria = false;
    int currentTenure = tenure;
    int iterCounter = 0; // Licznik iteracji do zarz¹dzania dywersyfikacj¹ i intensyfikacj¹
    int iterationsSinceLastDiversification = 0; // Licznik iteracji od ostatniej dywersyfikacji
    
    // Zmienna do przechowywania najlepszej œcie¿ki i jej kosztu przez ca³e uruchomienie
    vector<unsigned int> bestGlobalPath = globalPath;
    int bestFinalCost = finalCost;

    while (duration_cast<seconds>(high_resolution_clock::now() - startTime).count() < lifetime)
    {
        if (!aspirationCriteria)
        {
            findBestNeighbor(atspData, &path, &localCost, currentTenure, neighborSelection);
        }
        else
        {
            aspirationCriteria = false;
            findBestNeighbor(atspData, &path, &localCost, currentTenure / 2, neighborSelection);
        }

        if (localCost < finalCost)
        {
            //cout << "Nastapila poprawa " << localCost << endl;
            finalCost = localCost;
            globalPath = path;
            aspirationCriteria = true;
            iterationsSinceLastDiversification = 0; // Resetujemy licznik, jeœli znaleziono lepsze rozwi¹zanie
            
            if (localCost < bestFinalCost) // Aktualizacja najlepszej œcie¿ki, jeœli koszt jest lepszy
            {
                bestFinalCost = localCost;
                bestGlobalPath = path;
                bestSolutionTime = high_resolution_clock::now();
                cout
                    << duration_cast<seconds>(high_resolution_clock::now() - startTime).count() << " - " << float(bestFinalCost - expectedValue) / expectedValue * 100.0 << endl;
            }
        }
        else
        {
            iterationsSinceLastDiversification++; // Zwiêkszamy licznik, jeœli nie znaleziono poprawy
        }

        

        // Dywersyfikacja: po okreœlonej liczbie iteracji, gdy brak poprawy, wykonaj dywersyfikacjê
        if (iterationsSinceLastDiversification >= TS_maxNoImprovement)
        {
            //cout << "Reset" << endl;
            diversification(atspData); // Wywo³anie funkcji dywersyfikacji
            iterationsSinceLastDiversification = 0; // Resetujemy licznik
        }

        // Intensyfikacja: po iteracjach zmniejszamy tenurê
        if (++iterCounter > iterCountermax)
        {
            // Zwiêkszamy intensyfikacjê przez zmniejszenie tenury
            currentTenure = max(1, currentTenure - 1); // Zmniejszamy tenurê, ale nie do 0
            iterCounter = 0; // Resetujemy licznik iteracji
        }
        
    }
    finalCost = bestFinalCost;
    globalPath = bestGlobalPath;

    cout << "Najlepszy czas w : "
        << duration_cast<seconds>(bestSolutionTime - startTime).count()
        << " sekund od uruchomienia" << endl;
    cout << (char)7 << endl;
}

// Dywersyfikacja przez wygenerowanie nowej œcie¿ki startowej, uwzglêdniaj¹c ró¿nice w stosunku do globalPath
void TabuSearch::diversification(ATSPData atspData)
{
    // Generowanie listy dostêpnych wierzcho³ków
    vector<unsigned int> availableVertices(atspData.dimension);
    std::iota(availableVertices.begin(), availableVertices.end(), 0); // Wype³nienie 0...n-1

    // Przygotowanie generatora losowego
    std::random_device rd;
    std::mt19937 gen(rd());

    vector<unsigned int> newPath;

    // Rozpoczynamy od czêœciowej œcie¿ki bazuj¹cej na globalPath
    vector<unsigned int> partialPath(globalPath.begin(), globalPath.end());

    // Zmiana niektórych elementów w stosunku do globalPath
    // Tworzymy now¹ œcie¿kê przez losowanie wierzcho³ków, które nie pojawi³y siê w globalPath
    for (unsigned int i = 0; i < globalPath.size(); ++i)
    {
        if (i % 2 == 0) // Zmieniamy co drugi wierzcho³ek
        {
            // Szukamy wierzcho³ka, który nie jest obecny w globalPath
            auto it = std::find(availableVertices.begin(), availableVertices.end(), globalPath[i]);
            if (it != availableVertices.end())
            {
                availableVertices.erase(it); // Usuwamy u¿yty wierzcho³ek
            }

            // Losujemy nowy wierzcho³ek z pozosta³ych dostêpnych
            if (!availableVertices.empty())
            {
                int randIdx = std::uniform_int_distribution<int>(0, availableVertices.size() - 1)(gen);
                unsigned int newVertex = availableVertices[randIdx];
                newPath.push_back(newVertex);
                availableVertices.erase(availableVertices.begin() + randIdx); // Usuwamy wybrany wierzcho³ek
            }
            else
            {
                break; // Jeœli nie ma wiêcej dostêpnych wierzcho³ków, koñczymy
            }
        }
        else
        {
            // Pozostawiamy elementy takie jak w globalPath
            newPath.push_back(globalPath[i]);
        }
    }

    // Jeœli nie uda³o siê wygenerowaæ nowej œcie¿ki, po prostu generujemy losow¹
    if (newPath.size() != globalPath.size())
    {
        generatePath(atspData); // Generujemy losow¹ startow¹ œcie¿kê
    }
    else
    {
        newPath.push_back(newPath.front()); // Upewniamy siê, ¿e œcie¿ka jest cykliczna
        globalPath = newPath; // Przypisanie nowej œcie¿ki jako globalnej
        finalCost = calculateCost(atspData, globalPath); // Obliczenie kosztu nowej œcie¿ki
    }

}

/// generowanie pseudolosowej startowej œcie¿ki
void TabuSearch::generatePath(ATSPData atspData)
{

    // Generowanie listy dostêpnych wierzcho³ków
    vector<unsigned int> availableVertices(atspData.dimension);
    std::iota(availableVertices.begin(), availableVertices.end(), 0); // Wype³nienie 0...n-1

    // Przygotowanie generatora losowego
    std::random_device rd;
    std::mt19937 gen(rd());
    globalPath.clear();

    shuffle(availableVertices.begin(), availableVertices.end(), gen); // Tasowanie wierzcho³ków
    globalPath = availableVertices;                                   // Dodanie losowej permutacji jako œcie¿ki
    globalPath.push_back(globalPath.front());                         // Dodanie powrotu do pierwszego wierzcho³ka, aby utworzyæ cykl Hamiltona
    finalCost = calculateCost(atspData, globalPath);
    //showPath(globalPath);
}

// Obliczanie kosztu œcie¿ki
int TabuSearch::calculateCost(ATSPData atspData, vector<unsigned int> path)
{
    int cost = 0;
    for (size_t i = 0; i < path.size() - 1; i++)
    {
        cost += atspData.distances[path[i]][path[i + 1]];
    }
    return cost;
}

void TabuSearch::findBestNeighbor(ATSPData atspData, vector<unsigned int>* path, int* localCost, int tenure, int neighborSelection)
{
    vector<unsigned int> pairTabu(3, 0);
    pairTabu.at(2) = tenure;
    pair<unsigned int, unsigned int> tempPair;
    int minCost = INT_MAX;
    int deltaValue = 0;

    for (int i = 1; i < path->size() - 1; i++)
    {
        for (int j = i + 1; j < path->size() - 1; j++)
        {
            switch (neighborSelection)
            {
            case 1:
                deltaValue = swapNeighborsDelta(atspData, path, i, j);
                break;
            case 2:
                deltaValue = insertNeighborsDelta(atspData, path, i, j);
                break;
            case 3:
                deltaValue = reverseNeighborsDelta(atspData, path, i, j);
                break;
            default:
                cerr << "Invalid neighbor selection" << endl;
                continue;
            }

            if (deltaValue < minCost)
            {
                if (!checkAspirationCriteria(i, j, path))
                {
                    if (*localCost + deltaValue >= finalCost)
                        continue;
                }

                tempPair.first = i;
                tempPair.second = j;
                minCost = deltaValue;
            }
        }
    }

    *localCost += minCost; // Aktualizacja lokalnego kosztu

    pairTabu.at(0) = path->at(tempPair.first);
    pairTabu.at(1) = path->at(tempPair.second);

    switch (neighborSelection)
    {
    case 1:
        swapNeighbors(path, tempPair.first, tempPair.second);
        break;
    case 2:
        insertNeighbors(path, tempPair.first, tempPair.second);
        break;
    case 3:
        reverseNeighbors(path, tempPair.first, tempPair.second);
        break;
    default:
        cerr << "Invalid neighbor selection" << endl;
        return;
    }

    decrementTabuList();
    tabuList.push_back(pairTabu);
}

// podliczanie zmiany kosztów po zamianie wierzcho³ków
int TabuSearch::swapNeighborsDelta(ATSPData& atspData, vector<unsigned int>* path, int i, int j)
{

    int subtractOldEdges = 0;
    int addNewEdges = 0;
    if (j - i == 1) 
    {
        subtractOldEdges += atspData.distances[path->at(i - 1)][path->at(i)];
        subtractOldEdges += atspData.distances[path->at(i)][path->at(j)];
        subtractOldEdges += atspData.distances[path->at(j)][path->at(j + 1)];

        addNewEdges += atspData.distances[path->at(i - 1)][path->at(j)];
        addNewEdges += atspData.distances[path->at(j)][path->at(i)];
        addNewEdges += atspData.distances[path->at(i)][path->at(j + 1)];

    }
    else 
    {
        subtractOldEdges += atspData.distances[path->at(i - 1)][path->at(i)];
        subtractOldEdges += atspData.distances[path->at(i)][path->at(i + 1)];
        subtractOldEdges += atspData.distances[path->at(j - 1)][path->at(j)];
        subtractOldEdges += atspData.distances[path->at(j)][path->at(j + 1)];

        addNewEdges += atspData.distances[path->at(i - 1)][path->at(j)];
        addNewEdges += atspData.distances[path->at(j)][path->at(i + 1)];
        addNewEdges += atspData.distances[path->at(j - 1)][path->at(i)];
        addNewEdges += atspData.distances[path->at(i)][path->at(j + 1)];
    }

    return addNewEdges - subtractOldEdges;
}

void TabuSearch::swapNeighbors(vector<unsigned int>* path, int i, int j)
{
    swap((*path)[i], (*path)[j]);
}

int TabuSearch::insertNeighborsDelta(ATSPData& atspData, vector<unsigned int>* path, int i, int j) 
{
    if (i == j || i < 0 || j < 0 || i >= path->size() || j >= path->size()) {
        std::cerr << "Invalid indices for delta calculation." << std::endl;
        return INT_MAX; // Zwróæ bardzo du¿¹ wartoœæ w przypadku b³êdu.
    }

    int delta = 0;

    // Koszty zwi¹zane z usuniêciem elementu z pozycji j
    int prevIndex = (j == 0) ? path->size() - 1 : j - 1;
    int nextIndex = (j == path->size() - 1) ? 0 : j + 1;

    delta -= atspData.distances[path->at(prevIndex)][path->at(j)];
    delta -= atspData.distances[path->at(j)][path->at(nextIndex)];
    delta += atspData.distances[path->at(prevIndex)][path->at(nextIndex)];

    // Koszty zwi¹zane z wstawieniem elementu na pozycjê i
    int insertionPrev = (i == 0) ? path->size() - 1 : i - 1;

    delta -= atspData.distances[path->at(insertionPrev)][path->at(i)];
    delta += atspData.distances[path->at(insertionPrev)][path->at(j)];
    delta += atspData.distances[path->at(j)][path->at(i)];

    return delta;
}

void TabuSearch::insertNeighbors(vector<unsigned int>* path, int i, int j) 
{
    if (i == j || i < 0 || j < 0 || i >= path->size() || j >= path->size()) 
    {
        std::cerr << "Invalid indices for insertion operation." << std::endl;
        return;
    }

    // Przechowaj wartoœæ elementu na pozycji j
    unsigned int element = path->at(j);

    // Usuñ element z pozycji j
    path->erase(path->begin() + j);

    // Dostosuj indeks i, jeœli element zosta³ usuniêty przed wstawk¹
    if (j < i) i--;

    // Wstaw element na pozycjê i
    path->insert(path->begin() + i, element);
}

int TabuSearch::reverseNeighborsDelta(ATSPData& atspData, vector<unsigned int>* path, int i, int j)
{
    int n = path->size();  // Liczba wierzcho³ków w cyklu

    if (i == j || i < 0 || j < 0 || i >= n || j >= n) 
    {
        std::cerr << "Invalid indices for delta calculation." << std::endl;
        return INT_MAX;  // Zwróæ bardzo du¿¹ wartoœæ w przypadku b³êdu
    }

    int delta = 0;

    // Koszty zwi¹zane z usuniêciem krawêdzi przed i i po j
    int beforeI = (*path)[(i - 1 + n) % n];  // Wierzcho³ek przed i
    int afterJ = (*path)[(j + 1) % n];  // Wierzcho³ek po j
    delta -= atspData.distances[beforeI][(*path)[i]];
    delta -= atspData.distances[(*path)[j]][afterJ];

    // Koszty zwi¹zane z dodaniem krawêdzi po odwróceniu segmentu
    delta += atspData.distances[beforeI][(*path)[j]];  // KrawêdŸ przed i -> j
    delta += atspData.distances[(*path)[i]][afterJ];  // KrawêdŸ po j -> i

    return delta;
}
void TabuSearch::reverseNeighbors(vector<unsigned int>* path, int i, int j)
{
    while (i < j)
    {
        swap(path->at(i), path->at(j));
        i++;
        j--;
    }
}

// wyœwietlanie œcie¿ki
void TabuSearch::showPath(vector<unsigned int> path) 
{
    for (int i = 0; i < path.size() - 1; i++)
    {
        cout << path.at(i) << "->";
    }
    cout << path.back() << "\n";
}

// Dekrementacja tabu listy
void TabuSearch::decrementTabuList() 
{
    for (int i = 0; i < tabuList.size(); i++) 
    {
        tabuList.at(i).at(2)--;
        if (tabuList.at(i).at(2) == 0) 
        {
            tabuList.erase(tabuList.begin() + i);
            i -= 1;
        }
    }
}

// Sprawdzenie czy krawêdŸ znajduje siê na tabu liœcie
bool TabuSearch::checkAspirationCriteria(int i, int j,const vector<unsigned int>*  path)
{
    for (vector<unsigned int> v : tabuList) 
    {
        if (v.at(0) == path->at(i) && v.at(1) == path->at(j))
            return false;
        if (v.at(0) == path->at(j) && v.at(1) == path->at(i))
            return false;
    }
    return true;
}

// Getter kosztu koñcowego
int TabuSearch::getFinalCost() const
{
    return finalCost;
}

// Getter najlepszej œcie¿ki
vector<unsigned int> TabuSearch::getBestPath() const
{
    return globalPath;
}