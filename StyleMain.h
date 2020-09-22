#ifndef STYLE_MAIN_H_
#define STYLE_MAIN_H_

#include "Scanner.h"
#include "LosslessScanner.h"
#include <fstream>
#include <sstream>
#include "BraceChecker.h"
#include "IndentChecker.h"
#include "NonConstGlobalChecker.h"
#include "SingleLetterVarWarner.h"
#include <string>

#ifdef DEBUG
#include <iostream>
#endif

struct StyleFeedbackResponse {
  bool passed;
  bool warned;
  std::string msg;
  StyleFeedbackResponse(bool b, bool w, const std::string& m)
    : passed(b), warned(w), msg(m) {}
};

StyleFeedbackResponse getStyleFeedback(const std::string &fileName) {
  bool passed = true;
  bool warned = false;
  std::string warnedMsg;
  std::ifstream in(fileName);
  std::ostringstream out;

  std::vector<LosslessToken> losslessTokens = losslessScan(in);
  in.clear();
  in.seekg(0, in.beg);
  std::vector<Lossy::Token> tokens = scan(in);

#ifdef DEBUG
  for (Lossy::Token t : tokens) {
    std::cout << tokenToStr(t);
  }
#endif

  int missingBraceLine = getFirstMissingBraceLine(tokens);
  if (missingBraceLine != -1) {
    passed = false;
    out << "ERROR: Line " << missingBraceLine << ": Expected open brace\n";
    return StyleFeedbackResponse(passed, warned, out.str());
  }

  int singleLetterVarLine = getFirstSingleLetterVarLine(losslessTokens);
  if (singleLetterVarLine != -1) {
    warned = true;
    warnedMsg = "WARNING: Line " + std::to_string(singleLetterVarLine)
            + ": Single-letter variable name outside of a for loop\n"
            "\tConsider declaring for loop iterators in their respective for loops\n"
            "\t or using a more descriptive name\n";
  }

  int diffStyleLine = getFirstDiffBraceStyleLine(tokens);
  if (diffStyleLine != -1) {
    passed = false;
    out << "ERROR: Line " << diffStyleLine << ": Different brace style\n";
    if (warned) {
      out << warnedMsg;
    }
    return StyleFeedbackResponse(passed, warned, out.str());
  }

  int badIndentLine = getFirstBadIndentLine(tokens);
  if (badIndentLine != -1) {
    passed = false;
    out << "ERROR: Line " << badIndentLine << ": Bad indentation\n";
    if (warned) {
      out << warnedMsg;
    }
    return StyleFeedbackResponse(passed, warned, out.str());
  }

  int nonConstGlobalLine = getFirstNonConstGlobalLine(tokens);
  if (nonConstGlobalLine != -1) {
    passed = false;
    out << "ERROR: Line " << nonConstGlobalLine << ": Non-constant global variable\n";
    if (warned) {
      out << warnedMsg;
    }
    return StyleFeedbackResponse(passed, warned, out.str());
  }

  if (warned) {
    out << warnedMsg;
  }

  if (passed && !warned) {
    out << "No style issues detected\n";
  }

  return StyleFeedbackResponse(passed, warned, out.str());
}

bool styleTestPassed(std::ofstream& testFeedback, const std::vector<std::string>& fileNames) {
  bool passedAll = true;
  bool havePrinted = false;
  for (const std::string &fileName : fileNames) {
    StyleFeedbackResponse res = getStyleFeedback("../" + fileName);
    if (!res.passed || res.warned) {
      if (havePrinted) {
	      testFeedback << '\n';
      }
      havePrinted = true;
      if (!res.passed) {
        passedAll = false;
      }
      if (fileNames.size() > 1) {
	      testFeedback << fileName << ":";
      }
      testFeedback << res.msg;
    }
  }
  if (!havePrinted) {
    testFeedback << "No auto-graded style issues detected";
  }
  return passedAll;
}

#endif
