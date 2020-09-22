#include "StyleMain.h"
#include <iostream>

void evaluate(const std::string& fileName) {
  StyleFeedbackResponse res = getStyleFeedback(fileName);
  //std::cout << res.msg << std::flush;

  std::cout << fileName;
  if (res.passed) {
    std::cout << " passed\n" << res.msg << std::flush;
  }
  else {
    std::cout << " didn't pass\n" << res.msg << std::flush;
  }
}


int main(int argc, char* argv[]) {
  /*
  std::string fileName = "sample.txt";
  if (argc > 1) {
    fileName = argv[1];
  }*/

  std::vector<std::string> fileNames = {
          "main.cpp",
          "BakedGood.h",
          "BakedGood.cpp",
          "Bread.h",
          //"Bread.cpp",
          "Cake.h",
          "Cake.cpp",
          "CupCake.h",
          "CupCake.cpp",
          "LayerCake.h",
          "LayerCakeCake.cpp"
  };

  for (std::string fileName : fileNames) {
    evaluate("./lab/" + fileName);
  }

  return 0;
}

