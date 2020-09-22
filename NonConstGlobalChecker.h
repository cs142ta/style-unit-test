#ifndef NON_CONST_GLOBAL_CHECKER_H_
#define NON_CONST_GLOBAL_CHECKER_H_

#include <string>
#include <stack>
#include <algorithm>
#include "Token.h"

#ifdef DEBUG
#include <iostream>
#endif

struct GlobalsMachineData {
  int firstBadLine;
  int currLineNum;
  std::string currWS;
  int namespaceDepth;
  int braceDepth;
  std::vector<Lossy::Token> currNormalLine;

  GlobalsMachineData()
  : firstBadLine(-1),
    currLineNum(1),
    currWS("\n"),
    namespaceDepth(0),
    braceDepth (0) {}

  int badLineNum() {
    return firstBadLine;
  }
  void checkForNonConst() {
    std::vector<Lossy::Token>::const_iterator begin = currNormalLine.cbegin();
    std::vector<Lossy::Token>::const_iterator end = currNormalLine.cend();
    if (std::find(begin, end, Lossy::CONST_T) == end
        && std::find(begin, end, Lossy::CONSTEXPR_T) == end
        && std::find(begin, end, Lossy::USING_T) == end
        && std::find(begin, end, Lossy::TYPEDEF_T) == end
        && !isAFunctionHeadLine(currNormalLine)) {
      firstBadLine = currLineNum;
    }
  }
  void stepWS(Lossy::Token t) {
    if (t == Lossy::NEWLINE_T) {
      currLineNum++;
    }
  }
};

#ifdef DEBUG
const char* nonConstCheckerStateToStr(int s) {
  switch (s) {
    case 0:
      return "START_S";
    case 1:
      return "STRUCTURE_START_LINE_S";
    case 2:
      return "STRUCTURE_BODY_S";
    case 3:
      return "STRUCTURE_CLOSE_LINE_S";
    case 4:
      return "NORMAL_LINE_S";
    case 5:
      return "FUNC_BODY_S";
    case 6:
      return "BRACES_IN_NORMAL_LINE_S";
    case 7:
      return "NAMESPACE_START_LINE_S";
    case 8:
      return "PREPROC_LINE_S";
    default:
      return "??";
  }
}
#endif

int getFirstNonConstGlobalLine(const std::vector<Lossy::Token>& tokens) {
  GlobalsMachineData data;
  enum State {
    START_S,
    STRUCTURE_START_LINE_S,
    STRUCTURE_BODY_S,
    STRUCTURE_CLOSE_LINE_S,
    NORMAL_LINE_S,
    FUNC_BODY_S,
    BRACES_IN_NORMAL_LINE_S,
    NAMESPACE_START_LINE_S,
    PREPROC_LINE_S,
  } currState = START_S;
  for (Lossy::Token t : tokens) {
    //printf("State:%s, Token:%s\n", nonConstCheckerStateToStr(currState), tokenToStr(t));
    switch (currState) {
    case START_S:
      switch (t) {
      case Lossy::SINGLE_SPACE_T:
      case Lossy::TAB_T:
      case Lossy::NEWLINE_T:
        break;//just needed to stepws

      case Lossy::CLASS_T:
      case Lossy::STRUCT_T:
      case Lossy::UNION_T:
      case Lossy::ENUM_T:
        currState = STRUCTURE_START_LINE_S;
        break;

      case Lossy::OCTOTHORPE_T:
        currState = PREPROC_LINE_S;
        break;

      case Lossy::NAMESPACE_T:
	      currState = NAMESPACE_START_LINE_S;
	      break;

      case Lossy::R_BRACE_T:
        data.namespaceDepth--;
        break;
	
      case Lossy::EOF_T:
        break; //do nothing

      case Lossy::CASE_T:
      case Lossy::DEFAULT_T:
      case Lossy::PUBLIC_T:
      case Lossy::PRIVATE_T:
      case Lossy::PROTECTED_T:
      case Lossy::SEMICOLON_T:
      case Lossy::L_BRACE_T:
      case Lossy::ELSE_T:
      case Lossy::SWITCH_T:
      case Lossy::CATCH_T:
      case Lossy::IF_T:
      case Lossy::WHILE_T:
      case Lossy::FOR_T:
      case Lossy::DO_T:
      case Lossy::TRY_T:
      case Lossy::L_PAREN_T:
      case Lossy::R_PAREN_T:
      case Lossy::SINGLE_COLON_T:
      case Lossy::OTHER_STUFF_T:
      default: //shouldn't happen, but same as above
        data.currNormalLine.clear();
        data.currNormalLine.push_back(t);
        currState = NORMAL_LINE_S;
        break;
      }
      break;


    case STRUCTURE_START_LINE_S:
      if (t == Lossy::L_BRACE_T) {
        data.braceDepth = 1;
        currState = STRUCTURE_BODY_S;
      }
      break;

    case STRUCTURE_BODY_S:
      if (t == Lossy::L_BRACE_T) {
        data.braceDepth++;
      }
      else if (t == Lossy::R_BRACE_T) {
        data.braceDepth--;
        if (data.braceDepth == 0) {
          currState = STRUCTURE_CLOSE_LINE_S;
        }
      }
      break;

    case STRUCTURE_CLOSE_LINE_S:
      if (t == Lossy::SEMICOLON_T) {
        currState = START_S;
      }
      break;

    case NORMAL_LINE_S:
      data.currNormalLine.push_back(t);
      if (t == Lossy::SEMICOLON_T) {
        currState = START_S;
        data.checkForNonConst();
        if (data.badLineNum() != -1) {
          return data.badLineNum();
        }
      }
      else if (t == Lossy::L_BRACE_T) {
        data.braceDepth = 1;
        if (isBraceInLineFunctionOpen(data.currNormalLine)) {
          currState = FUNC_BODY_S;
        }
        else {
          currState = BRACES_IN_NORMAL_LINE_S;
        }
      }
      break;

    case FUNC_BODY_S:
      if (t == Lossy::L_BRACE_T) {
        data.braceDepth++;
      }
      else if (t == Lossy::R_BRACE_T) {
        data.braceDepth--;
        if (data.braceDepth == 0) {
          currState = START_S;
        }
      }
      break;

    case BRACES_IN_NORMAL_LINE_S:
      if (t == Lossy::L_BRACE_T) {
        data.braceDepth++;
      }
      else if (t == Lossy::R_BRACE_T) {
        data.braceDepth--;
        if (data.braceDepth == 0) {
          currState = NORMAL_LINE_S;
        }
      }
      break;

    case NAMESPACE_START_LINE_S:
      if (t == Lossy::L_BRACE_T) {
        data.namespaceDepth++;
        currState = START_S;
      }
      break;

    case PREPROC_LINE_S:
      if (t == Lossy::NEWLINE_T) {
        currState = START_S;
      }
      break;
    }
    data.stepWS(t);
  }
  return data.badLineNum();
}

#endif
