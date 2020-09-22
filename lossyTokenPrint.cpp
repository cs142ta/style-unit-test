#include "Scanner.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
  std::string fileName = "sample.txt";
  if (argc > 1) {
    fileName = argv[1];
  }
  std::ifstream in(fileName);

  std::vector<Lossy::Token> tokens = scan(in);

  for (Lossy::Token token : tokens) {
    std::cout << tokenToStr(token);// << std::endl;
  }
}