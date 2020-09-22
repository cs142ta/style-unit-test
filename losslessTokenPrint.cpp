#include "LosslessScanner.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
  std::string fileName = "sample.txt";
  if (argc > 1) {
    fileName = argv[1];
  }
  std::ifstream in(fileName);

  std::vector<LosslessToken> tokens = losslessScan(in);

  for (LosslessToken token : tokens) {
    std::cout << token.toString() << std::endl;
  }
}