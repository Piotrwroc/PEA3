#include "TabuSearch.h"

using namespace std;
using namespace chrono;

void TabuSearch::beginTabuSearch(ATSPData atspData, int lifetime, int tenure, int neighborSelection, int TS_maxNoImprovement)
{
    generatePath(atspData);

    mainLoop(atspData, lifetime, tenure, neighborSelection, TS_maxNoImprovement);
}

// G��wna p�tla programu
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
    int iterCounter = 0; // Licznik iteracji do zarz�dzania dywersyfikacj� i intensyfikacj�
    int iterationsSinceLastDiversification = 0; // Licznik iteracji od ostatniej dywersyfikacji
    
    // Zmienna do przechowywania najlepszej �cie�ki i jej kosztu przez ca�e uruchomienie
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
            iterationsSinceLastDiversification = 0; // Resetujemy licznik, je�li znaleziono lepsze rozwi�zanie
            
            if (localCost < bestFinalCost) // Aktualizacja najlepszej �cie�ki, je�li koszt jest lepszy
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
            iterationsSinceLastDiversification++; // Zwi�kszamy licznik, je�li nie znaleziono poprawy
        }

        

        // Dywersyfikacja: po okre�lonej liczbie iteracji, gdy brak poprawy, wykonaj dywersyfikacj�
        if (iterationsSinceLastDiversification >= TS_maxNoImprovement)
        {
            //cout << "Reset" << endl;
            diversification(atspData); // Wywo�anie funkcji dywersyfikacji
            iterationsSinceLastDiversification = 0; // Resetujemy licznik
        }

        // Intensyfikacja: po iteracjach zmniejszamy tenur�
        if (++iterCounter > iterCountermax)
        {
            // Zwi�kszamy intensyfikacj� przez zmniejszenie tenury
            currentTenure = max(1, currentTenure - 1); // Zmniejszamy tenur�, ale nie do 0
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

// Dywersyfikacja przez wygenerowanie nowej �cie�ki startowej, uwzgl�dniaj�c r�nice w stosunku do globalPath
void TabuSearch::diversification(ATSPData atspData)
{
    // Generowanie listy dost�pnych wierzcho�k�w
    vector<unsigned int> availableVertices(atspData.dimension);
    std::iota(availableVertices.begin(), availableVertices.end(), 0); // Wype�nienie 0...n-1

    // Przygotowanie generatora losowego
    std::random_device rd;
    std::mt19937 gen(rd());

    vector<unsigned int> newPath;

    // Rozpoczynamy od cz�ciowej �cie�ki bazuj�cej na globalPath
    vector<unsigned int> partialPath(globalPath.begin(), globalPath.end());

    // Zmiana niekt�rych element�w w stosunku do globalPath
    // Tworzymy now� �cie�k� przez losowanie wierzcho�k�w, kt�re nie pojawi�y si� w globalPath
    for (unsigned int i = 0; i < globalPath.size(); ++i)
    {
        if (i % 2 == 0) // Zmieniamy co drugi wierzcho�ek
        {
            // Szukamy wierzcho�ka, kt�ry nie jest obecny w globalPath
            auto it = std::find(availableVertices.begin(), availableVertices.end(), globalPath[i]);
            if (it != availableVertices.end())
            {
                availableVertices.erase(it); // Usuwamy u�yty wierzcho�ek
            }

            // Losujemy nowy wierzcho�ek z pozosta�ych dost�pnych
            if (!availableVertices.empty())
            {
                int randIdx = std::uniform_int_distribution<int>(0, availableVertices.size() - 1)(gen);
                unsigned int newVertex = availableVertices[randIdx];
                newPath.push_back(newVertex);
                availableVertices.erase(availableVertices.begin() + randIdx); // Usuwamy wybrany wierzcho�ek
            }
            else
            {
                break; // Je�li nie ma wi�cej dost�pnych wierzcho�k�w, ko�czymy
            }
        }
        else
        {
            // Pozostawiamy elementy takie jak w globalPath
            newPath.push_back(globalPath[i]);
        }
    }

    // Je�li nie uda�o si� wygenerowa� nowej �cie�ki, po prostu generujemy losow�
    if (newPath.size() != globalPath.size())
    {
        generatePath(atspData); // Generujemy losow� startow� �cie�k�
    }
    else
    {
        newPath.push_back(newPath.front()); // Upewniamy si�, �e �cie�ka jest cykliczna
        globalPath = newPath; // Przypisanie nowej �cie�ki jako globalnej
        finalCost = calculateCost(atspData, globalPath); // Obliczenie kosztu nowej �cie�ki
    }

}

/// generowanie pseudolosowej startowej �cie�ki
void TabuSearch::generatePath(ATSPData atspData)
{

    // Generowanie listy dost�pnych wierzcho�k�w
    vector<unsigned int> availableVertices(atspData.dimension);
    std::iota(availableVertices.begin(), availableVertices.end(), 0); // Wype�nienie 0...n-1

    // Przygotowanie generatora losowego
    std::random_device rd;
    std::mt19937 gen(rd());
    globalPath.clear();

    shuffle(availableVertices.begin(), availableVertices.end(), gen); // Tasowanie wierzcho�k�w
    globalPath = availableVertices;                                   // Dodanie losowej permutacji jako �cie�ki
    globalPath.push_back(globalPath.front());                         // Dodanie powrotu do pierwszego wierzcho�ka, aby utworzy� cykl Hamiltona
    finalCost = calculateCost(atspData, globalPath);
    //showPath(globalPath);
}

// Obliczanie kosztu �cie�ki
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

// podliczanie zmiany koszt�w po zamianie wierzcho�k�w
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
        return INT_MAX; // Zwr�� bardzo du�� warto�� w przypadku b��du.
    }

    int delta = 0;

    // Koszty zwi�zane z usuni�ciem elementu z pozycji j
    int prevIndex = (j == 0) ? path->size() - 1 : j - 1;
    int nextIndex = (j == path->size() - 1) ? 0 : j + 1;

    delta -= atspData.distances[path->at(prevIndex)][path->at(j)];
    delta -= atspData.distances[path->at(j)][path->at(nextIndex)];
    delta += atspData.distances[path->at(prevIndex)][path->at(nextIndex)];

    // Koszty zwi�zane z wstawieniem elementu na pozycj� i
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

    // Przechowaj warto�� elementu na pozycji j
    unsigned int element = path->at(j);

    // Usu� element z pozycji j
    path->erase(path->begin() + j);

    // Dostosuj indeks i, je�li element zosta� usuni�ty przed wstawk�
    if (j < i) i--;

    // Wstaw element na pozycj� i
    path->insert(path->begin() + i, element);
}

int TabuSearch::reverseNeighborsDelta(ATSPData& atspData, vector<unsigned int>* path, int i, int j)
{
    int n = path->size();  // Liczba wierzcho�k�w w cyklu

    if (i == j || i < 0 || j < 0 || i >= n || j >= n) 
    {
        std::cerr << "Invalid indices for delta calculation." << std::endl;
        return INT_MAX;  // Zwr�� bardzo du�� warto�� w przypadku b��du
    }

    int delta = 0;

    // Koszty zwi�zane z usuni�ciem kraw�dzi przed i i po j
    int beforeI = (*path)[(i - 1 + n) % n];  // Wierzcho�ek przed i
    int afterJ = (*path)[(j + 1) % n];  // Wierzcho�ek po j
    delta -= atspData.distances[beforeI][(*path)[i]];
    delta -= atspData.distances[(*path)[j]][afterJ];

    // Koszty zwi�zane z dodaniem kraw�dzi po odwr�ceniu segmentu
    delta += atspData.distances[beforeI][(*path)[j]];  // Kraw�d� przed i -> j
    delta += atspData.distances[(*path)[i]][afterJ];  // Kraw�d� po j -> i

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

// wy�wietlanie �cie�ki
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

// Sprawdzenie czy kraw�d� znajduje si� na tabu li�cie
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

// Getter kosztu ko�cowego
int TabuSearch::getFinalCost() const
{
    return finalCost;
}

// Getter najlepszej �cie�ki
vector<unsigned int> TabuSearch::getBestPath() const
{
    return globalPath;
}