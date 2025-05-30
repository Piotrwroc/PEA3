#include <iostream>
#include <cstdlib>
#include <fstream>
#include <climits>
#include <random>
#include <chrono>

#include "Simulation.h"
#include "Menu.h"
#include "LoadData.h"
#include "TabuSearch.h"
#include "SimulatedAnnealing.h"



#define REPETITION 5
#define UNIT 1000

using namespace std;

void Simulation::simulation_mode(int algnum)
{
    ATSPData data;

    int rodzaj_Sasiedztwa = 1;
    int rodzaj_Chlodzenia = 0;
    int tenure = 50;

    int czas_Graniczny[] = {120, 240, 360 }; // liczby wierzcho³ków

    std::string ListaPlikow[] = {
                           "ftv55.atsp",
                           "ftv170.atsp",
                           "rgb358.atsp"
    };
    //int vertexCounts[] = {50}; // liczby wierzcho³ków

    cout << alg_name(algnum) << endl;
    cout << TS_mode_name(rodzaj_Sasiedztwa) << endl;

    Menu menu;

    for (int n = 0; n < sizeof(ListaPlikow); n++)
    {
        cout << "--- Plik " << ListaPlikow[n] << endl;

        std::chrono::duration<double> czas{};


        for (int i = 0; i < REPETITION; i++)
        {
            data = loadATSPData(ListaPlikow[n]);


             if (algnum == 1)
            {
                TabuSearch tabuSearch(&data);
                tabuSearch.beginTabuSearch(data, czas_Graniczny[n], tenure, 1, 1000); // Uruchomienie algorytmu
                cout << tabuSearch.getFinalCost() << endl;
            }
            if (algnum == 2)
            {
                SimulatedAnnealing simulatedAnnealing(&data);
                simulatedAnnealing.beginSimulatedAnnealing(data, 0.99, czas_Graniczny[n]); // Uruchomienie algorytmu
            }
            if (algnum == 3)
            {
                //SA
                //
            }

        }

    }
    cout << endl << endl << endl;
}

const char* Simulation::alg_name(int algnum)
{
    switch (algnum)
    {
    case 1:
    {
        return "Algorytm Tabu Search";
        break;
    }
    case 2:
    {
        return "Algorytm Symulowanego Wyzarzania";
        break;
    }
    case 3:
    {
        return "Algorytm Zachlanny";
        break;
    }
    {
    deafult:
        return "error";
    }
    }
}

const char* Simulation::TS_mode_name(int algnum)
{
    switch (algnum)
    {
    case 1:
    {
        return "Swap";
        break;
    }
    case 2:
    {
        return "Insert";
        break;
    }
    case 3:
    {
        return "Invert";
        break;
    }
    {
    deafult:
        return "error";
    }
    }
}

const char* Simulation::SA_mode_name(int algnum)
{
    switch (algnum)
    {
    case 1:
    {
        return "1";
        break;
    }
    case 2:
    {
        return "2";
        break;
    }
    case 3:
    {
        return "3";
        break;
    }
    {
    deafult:
        return "error";
    }
    }
}