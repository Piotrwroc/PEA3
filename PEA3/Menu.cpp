#include "Menu.h"
#include "LoadData.h"
#include "TabuSearch.h"
#include "SimulatedAnnealing.h"
#include "NearestNeighbor.h"
#include "ResultSaver.h"
#include "Simulation.h"
#include "GeneticAlgorithm.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <climits>
#include <random>


using namespace std;

void Menu::f_menu()
{
    ATSPData data;
    bool dataLoaded = false;
    bool x = true;
    int czasGraniczny = 240;
    int rodzajSasiedztwa = 0;
    int rodzajChlodzenia = 0;
    double wspMutacji = 0.01;
    double wspKrzyzowania = 0.8;
    int rodzajMutacji = 0;
    int rodzajKrzyzowania = 0;
    int wielkoscPopulacji = 0;
    int lastAlgorithm = 0;
    vector<unsigned> lastPath;
    int TS_maxNoImprovement; 

    while (x)
    {
        int wybor_opcji = 0;
        cout << endl;
        cout << "Autorem programu jest Piotr Jozefek 272311" << endl;
        cout << "Wybierz funkcje programu: " << endl;
        cout << "1. Wczytaj dane z pliku" << endl;
        cout << "2. Wyswietl dane" << endl;
        cout << "3. Wprowadzenie kryterium stopu" << endl;
        cout << "4. Obliczenie rozwiazania metoda zachlanna" << endl;
        cout << "5. Wybor sasiedztwa" << endl;
        cout << "6. Uruchomianie algorytmu TS dla wczytanych danych i ustawionych parametrow i wyswietlenie wynikow" << endl;
        cout << "(niezaimplementowane) 7. Ustawienie wspolczynnika zmiany temperatury dla SW" << endl;
        cout << "8. Uruchomianie algorytmu SW dla wczytanych danych i ustawionych parametrow i wyswietlenie wynikow" << endl;
        cout << "9. Zapis sciezki rozwiazania do pliku txt" << endl;
        cout << "10. Wczytanie sciezki rozwiazania z pliku txt i obliczenie na drogi na podstawie wczytanej tabeli kosztow z wczytanego pliku." << endl;
        cout << "11. Ustawienie wielkoœci populacji pocz¹tkowej" << endl;
        cout << "12. Ustawienie wspó³czynnika mutacji" << endl;
        cout << "13. Ustawienia wspó³czynnika krzy¿owania" << endl;
        cout << "14. Wybór metody krzy¿owania (opcjonalnie)" << endl;
        cout << "15. Wybór metody metody mutacji (opcjonalnie)" << endl;
        cout << "16. Uruchomianie algorytmu dla wczytanych danych i ustawionych parametrów i wyœwietlenie wyników" << endl;
        cout << "17. Symulacja" << endl;
        cout << "18. Wylaczenie programu" << endl;
        cout << endl;
        cin >> wybor_opcji;

        switch (wybor_opcji)
        {
        case 1:
        {
            string filename;
            cout << "Podaj nazwe pliku do odczytu: ";
            cin >> filename;
            try
            {
                data = loadATSPData(filename);
                dataLoaded = true;
                cout << "Dane zostaly wczytane poprawnie" << endl;
            }
            catch (const exception& ex) 
            {
                cerr << "Blad: " << ex.what() << endl;
            }
            break;
        }
        case 2:
        {
            if (dataLoaded) 
            {
                printATSPData(data);
            }
            else 
            {
                cout << "Dane nie zostaly jeszcze wczytane" << endl;
            }
            break;
        }
        case 3:
        {
            cout << "Podaj kryterium stopu w sekundach: ";
            cin >> czasGraniczny;
            break;
        }
        case 4:
        {
            if (!dataLoaded)
            {
                cout << "Najpierw zaladuj dane z pliku (opcja 1)." << endl;
                break;
            }
            try 
            {
                NearestNeighbor nearestNeighbor(&data);
                nearestNeighbor.start(data);

                // Wyœwietlenie wyników
                cout << "Najlepsze rozwiazanie: " << nearestNeighbor.getFinalCost() << endl;
                cout << "Sciezka: ";
                for (int vertex : nearestNeighbor.getBestPath())
                {
                cout << vertex << " ";
                }
                cout << endl;
            }
            catch (const exception& ex)
            {
                cerr << "Blad podczas uruchamiania algorytmu: " << ex.what() << endl;
            }
            break;
        }
        case 5:
        {
            int opcja_tb;
            cout << "Wybierz wspolczynnik zmiany temperatury: " << endl;
            cout << "1. swap(i, j) - zamiana miejscami itego elementu z jtym" << endl;
            cout << "2. insert(i, j) - przeniesienie jtego elementu na pozycje ita" << endl;
            cout << "3. reverse(i, j) aktualnie nie dziala - odwrocenie kolejnosci w podciagu zaczynajacym sie na itej pozycji i konczacym sie na pozycji jtej." << endl;
            cin >> opcja_tb;

            if (opcja_tb == 1)
            {
                rodzajSasiedztwa = 1;
                TS_maxNoImprovement = 5000;
            }   
            else if (opcja_tb == 2)
            {
                rodzajSasiedztwa = 2;
                TS_maxNoImprovement = 15000;
            } 
            else if (opcja_tb == 3)
            {
                rodzajSasiedztwa = 3;
            }    
            else 
                cout << "Niepoprawna opcja " << endl;
            break;
        }  
        case 6:
        {
            if (!dataLoaded)
            {
                cout << "Najpierw zaladuj dane z pliku (opcja 1)." << endl;
                break;
            }
            if (rodzajSasiedztwa == 0)
            {
                cout << "Niewybrany tryb sasiedztwa" << endl;
                break;
            }

            int tenure = 50;

            // Wywo³anie algorytmu Tabu Search
            try 
            {
                //cout << "Wczytano wymiar: " << data.dimension << endl;
                TabuSearch tabuSearch(&data);
                tabuSearch.beginTabuSearch(data, czasGraniczny, tenure, rodzajSasiedztwa, TS_maxNoImprovement); // Uruchomienie algorytmu

                // Wyœwietlenie wyników
                cout << "Najlepsze rozwiazanie: " << tabuSearch.getFinalCost() << endl;
                cout << "Sciezka: ";
                vector<unsigned int> bestPath = tabuSearch.getBestPath();
                for (int i = 0; i < bestPath.size(); i++)
                {
                    cout << bestPath[i];
                    if (i < bestPath.size()) 
                    {
                        cout << " ";
                    }
                }
                cout << endl;
                lastPath = tabuSearch.getBestPath();
                lastAlgorithm = 1;
            }
            catch (const exception& ex)
            {
                cerr << "Blad podczas uruchamiania algorytmu: " << ex.what() << endl;
            }         
            break;
        }
        case 7:
        {
            int opcja_sw;
            cout << "Wybierz wartosci wspolczynnika zmiany temperatury: " << endl;
            cout << "1" << endl; //
            cout << "2" << endl; //
            cout << "3" << endl; //
            cin >> opcja_sw;

            if (opcja_sw == 1)
                rodzajChlodzenia = 1;
            else if (opcja_sw == 2)
                rodzajChlodzenia = 2;
            else if (opcja_sw == 3)
                rodzajChlodzenia = 3;
            else
                cout << "Niepoprawna opcja " << endl;
            break;
        }
        case 8:
        {
            if (!dataLoaded)
            {
                cout << "Najpierw zaladuj dane z pliku (opcja 1)." << endl;
                break;
            }

            if (rodzajChlodzenia != 0)
            {
                cout << "Niewybrany tryb chlodzenia" << endl;
                break;
            }

            double const alfa = 0.99;

            // Wywo³anie algorytmu Symulowanego wyzarzania
            try {
                //cout << "Wczytano wymiar: " << data.dimension << endl;
                SimulatedAnnealing simulatedAnnealing(&data);
                simulatedAnnealing.beginSimulatedAnnealing(data, alfa, czasGraniczny); // Uruchomienie algorytmu

                // Wyœwietlenie wyników
                cout << "Najlepsze rozwiazanie: " << simulatedAnnealing.getFinalCost() << endl;
                cout << "Sciezka: ";
                for (int vertex : simulatedAnnealing.getBestPath())
                {
                    cout << vertex << " ";
                }
                cout << endl;
                cout << "Temperatura na koniec algorytmu: " << simulatedAnnealing.getCurrentTemperature() << endl;
                cout << "Wartosc wyrazenia exp(-1/Tk): " << simulatedAnnealing.getExponentialDecay() << endl;
                lastPath = simulatedAnnealing.getBestPath();
                lastAlgorithm = 2;
            }
            catch (const exception& ex)
            {
                cerr << "Blad podczas uruchamiania algorytmu: " << ex.what() << endl;
            }
            break;
        }
        case 9:
        {
            if (lastAlgorithm == 0) 
            {
                cout << "Nie uruchomiono jeszcze zadnego algorytmu" << endl;;
            }
            else 
            {
                string filename;
                cout << "Podaj nazwe pliku do zapisu: ";
                cin >> filename;

                // Zapisanie œcie¿ki do pliku
                ResultSaver::savePathToFile(filename, lastPath);
            }
            break;
        }
        case 10:
        {
            if (!dataLoaded)
            {
                cout << "Najpierw zaladuj dane z pliku (opcja 1)." << endl;
                break;
            }

            string filename;
            cout << "Podaj nazwe pliku do odczytu: ";
            cin >> filename;
            try
            {
                vector<int> path = loadPathFromFile(filename);  // Plik z rozwi¹zaniem (œcie¿k¹)

                // Obliczenie kosztu œcie¿ki
                int pathCost = calculatePathCostFromFile(data, path);

                // Wyœwietlenie wyniku
                cout << "Koszt sciezki: " << pathCost << endl;

            }
            catch (const exception& ex)
            {
                cerr << "Blad: " << ex.what() << endl;
            }
            break;      
        }
        case 11:
        {
            int opcja_pop;
            cout << "Wybierz wielkosc populacji: " << endl;
            cout << "1. 500" << endl;
            cout << "2. 1000" << endl;
            cout << "3. 5000" << endl;
            cin >> opcja_pop;

            if (opcja_pop == 1)
            {
                wielkoscPopulacji = 500;
            }
            else if (opcja_pop == 2)
            {
                wielkoscPopulacji = 1000;
            }
            else if (opcja_pop == 3)
            {
                wielkoscPopulacji = 5000;
            }
            else
                cout << "Niepoprawna opcja " << endl;

            break;
        }
        case 12:
        {
            cout << "Ustaw wpolczynnik mutacji: " << endl;
            cin >> wspMutacji;
            break;
        }
        case 13:
        {
            cout << "Ustaw wpolczynnik krzyzowania: " << endl;;
            cin >> wspKrzyzowania;
            break;
        }
        case 14:
        {
            int opcja_krz;
            cout << "Wybierz rodzaj krzyzowania: " << endl;
            cout << "1. Order crossover" << endl;
            cout << "2. Partially mapped crossover" << endl;

            cin >> opcja_krz;

            if (opcja_krz == 1)
            {
                rodzajKrzyzowania = 1;
            }
            else if (opcja_krz == 2)
            {
                rodzajKrzyzowania = 2;
            }
            else
                cout << "Niepoprawna opcja " << endl;

            break;
        }
        case 15:
        {
            int opcja_mu;
            cout << "Wybierz rodzaj mutacji: " << endl;
            cout << "1. swap(i, j) - zamiana miejscami itego elementu z jtym" << endl;
            cout << "2. insert(i, j) - przeniesienie jtego elementu na pozycje ita" << endl;
            cin >> opcja_mu;

            if (opcja_mu == 1)
            {
                rodzajMutacji = 1;
            }
            else if (opcja_mu == 2)
            {
                rodzajMutacji = 2;
            }
            else
                cout << "Niepoprawna opcja " << endl;

            break;
        }
        case 16:
        {     
                if (!dataLoaded)
                {
                    cout << "Najpierw zaladuj dane z pliku (opcja 1)." << endl;
                    break;
                }
                if (rodzajKrzyzowania == 0)
                {
                    cout << "Niewybrany rodzaj krzyzowania" << endl;
                    break;
                }
                if (rodzajMutacji == 0)
                {
                    cout << "Niewybrany rodzaj mutacji" << endl;
                    break;
                }
                if (wielkoscPopulacji == 0)
                {
                    cout << "Niewybrana wielkosc populacji" << endl;
                    break;
                }    
                    try
                    {
                        GeneticAlgorithm geneticAlgorithm(&data);
                        geneticAlgorithm.beginGenetic(data, czasGraniczny, wielkoscPopulacji, rodzajKrzyzowania, rodzajMutacji, wspKrzyzowania, wspMutacji); // Uruchomienie algorytmu
                        cout << "Najlepsze rozwiazanie: " << geneticAlgorithm.getFinalCost() << endl;
                        cout << "Sciezka: ";
                        vector<unsigned int> bestPath = geneticAlgorithm.getBestPath();
                        for (int i = 0; i < bestPath.size(); i++)
                        {
                            cout << bestPath[i];
                            cout << " ";
                        }
                        cout << bestPath.front() << endl;
                        cout << endl;
                        lastPath = geneticAlgorithm.getBestPath();
                        lastAlgorithm = 3;
                    }
                    catch (const exception& ex)
                    {
                        cerr << "Blad podczas uruchamiania algorytmu: " << ex.what() << endl;
                    }
                    break;   
        }
        case 17:
        {
            Simulation simulation;
            simulation.simulation_mode(1);
            break;
        }
        case 18:
        {
            cout << "Zakonczono dzialanie programu" << endl;
            exit(0);
            break;
        }

        default:
        {
            cout << "Wybrano niepoprawna opcje" << endl;
            break;
        }
        }
    }
}
