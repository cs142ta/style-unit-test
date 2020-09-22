#include "StyleMain.h"
#include <vector>
#include <string>

bool testPassed(std::ofstream& testFeedback) {
    std::vector<std::string> fileNames = {
        "main.cpp"
    };
    return styleTestPassed(testFeedback, fileNames);
}