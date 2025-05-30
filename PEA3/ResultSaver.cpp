#include "ResultSaver.h"

using namespace std;

void ResultSaver::savePathToFile(const string& filename, const vector<unsigned>& path)
{
    ofstream outFile(filename);
    if (!outFile)
    {
        cerr << "Nie udalo sie otworzyc pliku: " << filename << endl;
        return;
    }

    // Zapis iloœci wierzcho³ków
    outFile << path.size()-1 << "\n";

    // Zapis numerów wierzcho³ków w œcie¿ce
    for (const auto& vertex : path)
    {
        outFile << vertex << "\n";
    }

    outFile.close();
    cout << "Sciezka zostala zapisana do pliku: " << filename << endl;
}
