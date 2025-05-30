#include "NearestNeighbor.h"
#include "LoadData.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <climits>

using namespace std;

void NearestNeighbor::start(ATSPData atspData)
{
    int wierzcholki = atspData.dimension;                // Liczba wierzcho³ków z danych ATSP
    std::vector<std::vector<int>>& tab = atspData.distances; // Odwo³anie do macierzy odleg³oœci

    int min_wynik = INT_MAX;
    std::vector<unsigned> najlepsza_sciezka(wierzcholki);

    // Testujemy wszystkie wierzcho³ki startowe
    for (int wierz_start = 0; wierz_start < wierzcholki; wierz_start++)
    {
        std::vector<int> zajete(wierzcholki, 0);
        int wynik = 0, wierz_obecny = wierz_start, min = INT_MAX, waga = 0, wierz_min, id = 0;

        globalPath.push_back(wierz_obecny); // Dodajemy pierwszy wierzcho³ek do œcie¿ki

        for (int i = 0; i < wierzcholki - 1; i++)
        {
            min = INT_MAX;
            zajete[wierz_obecny] = 1;
            for (int j = 0; j < wierzcholki; j++)
            {
                waga = tab[wierz_obecny][j];
                if (zajete[j] == 0)
                {
                    if (waga < min)
                    {
                        wierz_min = j;
                        min = waga;
                    }
                }
            }
            wynik += min;
            wierz_obecny = wierz_min;
            globalPath.push_back(wierz_obecny); // Dodajemy kolejny wierzcho³ek do œcie¿ki
        }
        wynik += tab[wierz_obecny][wierz_start];
        globalPath.push_back(wierz_start); // Zakoñczenie œcie¿ki

        // Sprawdzamy, czy wynik dla tego wierzcho³ka jest minimalny
        if (wynik < min_wynik)
        {
            min_wynik = wynik;
            najlepsza_sciezka = globalPath; // Zapisujemy najlepsz¹ œcie¿kê
        }

        globalPath.clear(); // Czyszczenie œcie¿ki przed kolejn¹ iteracj¹
    }

    // Kopiowanie najlepszej œcie¿ki do globalnej œcie¿ki
    globalPath = najlepsza_sciezka;
    finalCost = min_wynik; // Przypisanie koñcowego wyniku
}

int NearestNeighbor::getFinalCost() const
{
    return finalCost;
}

std::vector<unsigned> NearestNeighbor::getBestPath() const
{
    return globalPath;
}
