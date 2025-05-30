#ifndef RESULT_SAVER_H
#define RESULT_SAVER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class ResultSaver
{
public:
    static void savePathToFile(const std::string& filename, const std::vector<unsigned>& path);
};

#endif // RESULT_SAVER_H
