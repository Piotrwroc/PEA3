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

    // Zapis ilo�ci wierzcho�k�w
    outFile << path.size()-1 << "\n";

    // Zapis numer�w wierzcho�k�w w �cie�ce
    for (const auto& vertex : path)
    {
        outFile << vertex << "\n";
    }

    outFile.close();
    cout << "Sciezka zostala zapisana do pliku: " << filename << endl;
}
