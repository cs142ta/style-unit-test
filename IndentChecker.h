#ifndef INDENT_CHECKER_H_
#define INDENT_CHECKER_H_

#include <string>
#include <stack>
#include "Token.h"

#ifdef DEBUG
#include <iostream>
#endif

int max(int a, int b, int c) {
  if (a > b
      && a > c) {
    return a;
  }
  else if (b > c) {
    return b;
  }
  else {
    return c;
  }
}

struct IndentMachineData {
  int firstBad2SpaceLine;
  int firstBad3SpaceLine;
  int firstBad4SpaceLine;
  int currLineNum;
  std::string currWS;
  std::stack<Lossy::Token> stack;
  int classDepth;
  int nonIndentingBraceDepth;
  int parenInStructureStartLineDepth;
  std::vector<Lossy::Token> currNormalLine;

  IndentMachineData()
  : firstBad2SpaceLine(-1),
    firstBad3SpaceLine(-1),
    firstBad4SpaceLine(-1),
    currLineNum(1),
    currWS("\n"),
    classDepth(0),
    nonIndentingBraceDepth(0),
    parenInStructureStartLineDepth(0) {}

  int numRealSpaces(int tabSize) {
    int res = 0;
    for (char c : currWS) {
      if (c != '\n') {
        res += (c == '\t') ? tabSize : 1;
      }
    }
    return res;
  }
  bool wsIsCorrect(int expectedNumLevels,
                   int tabSize) {
    return tabSize * expectedNumLevels
      == numRealSpaces(tabSize);
  }
  void evalIndent(int expectedNumLevels) {
    if (currWS.empty()
        || currWS.front() != '\n') {
      if (firstBad2SpaceLine == -1) {
        firstBad2SpaceLine = currLineNum;
      }
      if (firstBad3SpaceLine == -1) {
        firstBad3SpaceLine = currLineNum;
      }
      if (firstBad4SpaceLine == -1) {
        firstBad4SpaceLine = currLineNum;
      }
    }
    if (firstBad2SpaceLine == -1
        && !wsIsCorrect(expectedNumLevels, 2)) {
      firstBad2SpaceLine = currLineNum;
    }
    if (firstBad3SpaceLine == -1
        && !wsIsCorrect(expectedNumLevels, 3)) {
      firstBad3SpaceLine = currLineNum;
    }
    if (firstBad4SpaceLine == -1
        && !wsIsCorrect(expectedNumLevels, 4)) {
      firstBad4SpaceLine = currLineNum;
    }
#ifdef DEBUG
    printf("\tstacksize: %lu, expect %d, ws:",
	   stack.size(),
           expectedNumLevels);
    for (char c : currWS) {
      switch (c) {
      case ' ':
        std::cout << '_';
        break;
      case '\t':
        std::cout << '>';
        break;
      case '\n':
        std::cout << 'n';
        break;
      default:
        std::cout << '!';
        break;
      }
    }
    std::cout << "\n";
#endif
  }

  int baseDepth(bool indentAccess,
                bool indentPastAccess) {
    return stack.size()
      + ((indentAccess && indentPastAccess) ? classDepth : 0);
  }
  void evalNormalIndent(bool indentAccess,
                        bool indentPastAccess) {
#ifdef DEBUG
    std::cout << "evalNormal\n";
#endif
    evalIndent(baseDepth(indentAccess, indentPastAccess));
  }
  void evalPreProcIndent(bool indentAccess,
                        bool indentPastAccess) {
#ifdef DEBUG
    std::cout << "evalPreProc\n";
#endif
    evalIndent(0);
  }
  void evalLabelIndent(bool indentAccess,
                       bool indentPastAccess) {
#ifdef DEBUG
    std::cout << "evalLabel, baseDepth:"
            << baseDepth(indentAccess, indentPastAccess) << "\n";
#endif
    evalIndent(baseDepth(indentAccess, indentPastAccess)
               - (indentPastAccess ? 1 : 0));
  }
  void evalLBraceIndent(bool indentAccess,
                        bool indentPastAccess) {
#ifdef DEBUG
    std::cout << "evalLBrace\n";
#endif
    if (!currWS.empty()
        && currWS.front() == '\n') {
      int depth = baseDepth(indentAccess, indentPastAccess) - 1;
      Lossy::Token whatTheBraceOpens = stack.top();
      if (indentAccess && indentPastAccess
          && (whatTheBraceOpens == Lossy::CLASS_T
              || whatTheBraceOpens == Lossy::STRUCT_T
              || whatTheBraceOpens == Lossy::UNION_T
              || whatTheBraceOpens == Lossy::SWITCH_T)) {
        depth--;
      }
      evalIndent(depth);
    }
  }
  void evalElseIndent(bool indentAccess,
                      bool indentPastAccess) {
#ifdef DEBUG
    std::cout << "evalElse\n";
#endif
    if (!currWS.empty()
        && currWS.front() == '\n') {
      evalIndent(baseDepth(indentAccess, indentPastAccess));
    }
  }
  void evalIfAfterElseIndent(bool indentAccess,
                      bool indentPastAccess) {
#ifdef DEBUG
    std::cout << "evalIfAfterElse\n";
#endif
    if (!currWS.empty()
        && currWS.front() == '\n') {
      evalIndent(baseDepth(indentAccess, indentPastAccess));
    }
  }
  int badLineNum() {
    if (firstBad2SpaceLine == -1
        || firstBad3SpaceLine == -1
        || firstBad4SpaceLine == -1) {
      return -1;
    }
    else {
      return max(firstBad2SpaceLine,
                 firstBad3SpaceLine,
                 firstBad4SpaceLine);
    }
  }
  void stepWS(Lossy::Token t) {
    switch (t) {
    case Lossy::SINGLE_SPACE_T:
      //std::cout << "Add space\n";
      currWS.push_back(' ');
      break;
    case Lossy::TAB_T:
      //std::cout << "Add tab\n";
      currWS.push_back('\t');
      break;
    case Lossy::NEWLINE_T:
      //std::cout << "Clear, add nl\n";
      currLineNum++;
      currWS.clear();
      currWS.push_back('\n');
      break;
    default:
      //std::cout << "Clear\n";
      currWS.clear();
      break;
    }
  }
};

#define RET_IF_BAD                              \
  badIndentLine = data.badLineNum();            \
  if (badIndentLine != -1) {                    \
    return badIndentLine;                       \
  }

int getFirstBadIndentLine(const std::vector<Lossy::Token>& tokens,
                       bool indentAccess,
                       bool indentPastAccess);


int getFirstBadIndentLine(const std::vector<Lossy::Token>& tokens) {
#ifdef DEBUG
  std::cout << "\nBEGIN LR\n";
#endif
  int lrval = getFirstBadIndentLine(tokens, false, true);
#ifdef DEBUG
  std::cout << "\nEND LR\n";
  std::cout << "\nBEGIN RL\n";
#endif
  int rlval = getFirstBadIndentLine(tokens, true, false);
#ifdef DEBUG
  std::cout << "\nEND RL\n";
  std::cout << "\nBEGIN RR\n";
#endif
  int rrval = getFirstBadIndentLine(tokens, true, true);
#ifdef DEBUG
  std::cout << "\nEND RR\n";
#endif

  if (lrval == -1 || rlval == -1 || rrval == -1) {
    return -1;
  }
  else {
    return max(lrval, rlval, rrval);
  }
}

#ifdef DEBUG
const char* indentCheckerStateToStr(int s) {
  switch (s) {
    case 0:
      return "START_S";
    case 1:
      return "ELSE_OPTION_S";
    case 2:
      return "STRUCTURE_START_LINE_S";
    case 3:
      return "STRUCTURE_CLOSE_LINE_S";
    case 4:
      return "NORMAL_LINE_S";
    case 5:
      return "LABEL_LINE_S";
    case 6:
      return "PREPROC_LINE_S";
    case 7:
      return "WAIT_FOR_MATCHING_BRACE_S";
    case 8:
      return "WAIT_FOR_MATCHING_PAREN_S";
    default:
      return "??";
  }
}
#endif

int getFirstBadIndentLine(const std::vector<Lossy::Token>& tokens,
                       bool indentAccess,
                       bool indentPastAccess) {
  IndentMachineData data;
  enum State {
    START_S,
    ELSE_OPTION_S,
    STRUCTURE_START_LINE_S,
    STRUCTURE_CLOSE_LINE_S,
    NORMAL_LINE_S,
    LABEL_LINE_S,
    PREPROC_LINE_S,
    WAIT_FOR_MATCHING_BRACE_S,
    WAIT_FOR_MATCHING_PAREN_S,
  } currState = START_S;
  int badIndentLine = -1;
  Lossy::Token prevToken = Lossy::EOF_T;
  for (Lossy::Token t : tokens) {
    //printf("State:%s, Token:%s\n", indentCheckerStateToStr(currState), tokenToStr(t));
    switch (currState) {
    case START_S:
      switch (t) {
      case Lossy::SINGLE_SPACE_T:
      case Lossy::TAB_T:
      case Lossy::NEWLINE_T:
        break;//just needed to stepws

      case Lossy::CASE_T:
      case Lossy::DEFAULT_T:
      case Lossy::PUBLIC_T:
      case Lossy::PRIVATE_T:
      case Lossy::PROTECTED_T:
        data.evalLabelIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        currState = LABEL_LINE_S;
        break;

      case Lossy::SEMICOLON_T:
        data.evalNormalIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        break;

      case Lossy::L_BRACE_T:
        data.evalNormalIndent(indentAccess, indentPastAccess); //This is a scope open,
                                 //which will not be on a same line
        RET_IF_BAD;
        data.stack.push(t);
        break;

      case Lossy::R_BRACE_T:
        {
          Lossy::Token closing = data.stack.top();
          data.stack.pop();
          if (closing == Lossy::SWITCH_T
              || closing == Lossy::CLASS_T
              || closing == Lossy::STRUCT_T
              || closing == Lossy::UNION_T) {
            data.classDepth--;
          }
          if (prevToken != Lossy::L_BRACE_T) {
            data.evalNormalIndent(indentAccess, indentPastAccess);
            RET_IF_BAD;
          }
          switch (closing) {
          case Lossy::CLASS_T:
          case Lossy::STRUCT_T:
          case Lossy::UNION_T:
          case Lossy::DO_T:
	    //case ENUM_T: now handled by special case
            currState = STRUCTURE_CLOSE_LINE_S;
            break;
          default:
            currState = START_S;
            break;
          }
        }
        break;

      case Lossy::OCTOTHORPE_T:
        data.evalPreProcIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        currState = PREPROC_LINE_S;
        break;

      case Lossy::ELSE_T:
        data.evalElseIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        data.stack.push(t);
        currState = ELSE_OPTION_S;
        break;

      case Lossy::SWITCH_T:
      case Lossy::CLASS_T:
      case Lossy::STRUCT_T:
      case Lossy::UNION_T:
      case Lossy::CATCH_T:
      case Lossy::IF_T:
      case Lossy::WHILE_T:
      case Lossy::FOR_T:
      case Lossy::DO_T:
      case Lossy::TRY_T:
      case Lossy::NAMESPACE_T:
      case Lossy::ENUM_T:
        data.evalNormalIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        data.stack.push(t);
        if (t == Lossy::SWITCH_T
            || t == Lossy::CLASS_T
            || t == Lossy::STRUCT_T
            || t == Lossy::UNION_T) {
          data.classDepth++;
        }
        currState = STRUCTURE_START_LINE_S;
        break;

      case Lossy::EOF_T:
        break; //do nothing

      case Lossy::L_PAREN_T:
      case Lossy::R_PAREN_T:
      case Lossy::SINGLE_COLON_T:
      case Lossy::OTHER_STUFF_T:
      default: //shouldn't happen, but same as above
        data.evalNormalIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        data.currNormalLine.clear();
        data.currNormalLine.push_back(t);
        currState = NORMAL_LINE_S;
        break;
      }
      break;

    case ELSE_OPTION_S:
      if (t == Lossy::IF_T) {
        data.evalIfAfterElseIndent(indentAccess, indentPastAccess);
        data.stack.pop();
        data.stack.push(t);
        currState = STRUCTURE_START_LINE_S;
      }
      else if (t == Lossy::L_BRACE_T) {
        data.evalLBraceIndent(indentAccess, indentPastAccess);
        RET_IF_BAD;
        currState = START_S;
      }
      break;

    case STRUCTURE_START_LINE_S:
      if (t == Lossy::L_BRACE_T) {
        if (data.stack.top() == Lossy::ENUM_T) {
          data.stack.pop();
          data.nonIndentingBraceDepth = 1;
          currState = WAIT_FOR_MATCHING_BRACE_S;
        }
        else {
          data.evalLBraceIndent(indentAccess, indentPastAccess);
          RET_IF_BAD;
          currState = START_S;
        }
      }
      else if (t == Lossy::L_PAREN_T) {
        data.parenInStructureStartLineDepth = 1;
        currState = WAIT_FOR_MATCHING_PAREN_S;
      }
      else if (t == Lossy::SEMICOLON_T) {
        Lossy::Token closing = data.stack.top();
        data.stack.pop();
        if (closing == Lossy::SWITCH_T
            || closing == Lossy::CLASS_T
            || closing == Lossy::STRUCT_T
            || closing == Lossy::UNION_T) {
          data.classDepth--;
        }
        currState = START_S;
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
      }
      else if (t == Lossy::L_BRACE_T) {
        if (isBraceInLineOpeningIndentLevel(data.currNormalLine)) {
          data.stack.push(t);
          data.evalLBraceIndent(indentAccess, indentPastAccess);
          RET_IF_BAD;
          currState = START_S;
        }
        else {
          data.nonIndentingBraceDepth = 1;
          currState = WAIT_FOR_MATCHING_BRACE_S;
        }
      }
      break;

    case LABEL_LINE_S:
      if (t == Lossy::SINGLE_COLON_T) {
        currState = START_S;
      }
      break;

    case PREPROC_LINE_S:
      if (t == Lossy::NEWLINE_T) {
        currState = START_S;
      }
      break;

    case WAIT_FOR_MATCHING_BRACE_S:
      if (t == Lossy::L_BRACE_T) {
        data.nonIndentingBraceDepth++;
      }
      else if (t == Lossy::R_BRACE_T) {
        data.nonIndentingBraceDepth--;
        if (data.nonIndentingBraceDepth == 0) {
          currState = STRUCTURE_CLOSE_LINE_S;
        }
      }
      break;

    case WAIT_FOR_MATCHING_PAREN_S:
      if (t == Lossy::L_PAREN_T) {
        data.parenInStructureStartLineDepth++;
      }
      else if (t == Lossy::R_PAREN_T) {
        data.parenInStructureStartLineDepth--;
        if (data.parenInStructureStartLineDepth == 0) {
          currState = STRUCTURE_START_LINE_S;
        }
      }
      break;
    }

    data.stepWS(t);
    prevToken = t;
  }
  return data.badLineNum();
}

#endif
