#ifndef BRACE_CHECKER_H_
#define BRACE_CHECKER_H_

#include "Token.h"
#include <vector>
#include <stack>

int getFirstDiffBraceStyleLine(const std::vector<Lossy::Token>& tokens) {
  std::vector<int> rSameLines;
  std::vector<int> rNextLines;
  int rLineNum = 0;
  int rBraceLineNum = -1;
  bool sawNewline = false;

  for (std::vector<Lossy::Token>::const_reverse_iterator rit = tokens.crbegin();
       rit != tokens.crend();
       rit++) {
    if (*rit == Lossy::NEWLINE_T) {
      rLineNum++;
      sawNewline = true;
    }
    if (rBraceLineNum != -1 && !tokenIsws(*rit)) {
      if (*rit == Lossy::R_PAREN_T
          || *rit == Lossy::DO_T
          || *rit == Lossy::ELSE_T) {
        if (sawNewline) {
          rNextLines.push_back(rBraceLineNum);
        }
        else {
          rSameLines.push_back(rBraceLineNum);
        }
      }
      rBraceLineNum = -1;
    }
    if (*rit == Lossy::L_BRACE_T) {
      rBraceLineNum = rLineNum;
      sawNewline = false;
    }
  }

  std::vector<int>& smallerRList = (rSameLines.size() < rNextLines.size())
    ? rSameLines : rNextLines;

  if (smallerRList.empty()) {
    return -1;
  }
  else {
    return rLineNum - smallerRList.back() + 1;
  }
}
  

int getFirstMissingBraceLine(const std::vector<Lossy::Token>& tokens) {
  std::stack<Lossy::Token> stack;
  enum State {
    WAIT_S,
    L_PAREN_S,
    LAST_OTHER_S,
    BRACE_S,
    ELSE_OPTION_S,
    FORS_L_PAREN_S,
    FORS_INIT_S,
    FORS_CONDITION_S,
    DOS_WHILE_S,
  } currState = WAIT_S;
  int lineNum = 1;

  for (Lossy::Token t : tokens) {
    if (t == Lossy::NEWLINE_T) {
      lineNum++;
    }
    if (tokenIsws(t)) {
      continue;
    }
    switch (currState) {
    case WAIT_S:
      switch (t) {
      case Lossy::FOR_T:
        stack.push(t);
        currState = FORS_L_PAREN_S;
        break;
      case Lossy::IF_T:
      case Lossy::WHILE_T:
      case Lossy::SWITCH_T:
        stack.push(t);
        currState = L_PAREN_S;
        break;
      case Lossy::ELSE_T:
        stack.push(t);
        currState = ELSE_OPTION_S;
        break;
      case Lossy::DO_T:
        stack.push(t);
        currState = BRACE_S;
        break;
      case Lossy::R_BRACE_T:
        if (stack.top() == Lossy::DO_T) {
          currState = DOS_WHILE_S;
        }
        stack.pop();
        break;
      case Lossy::L_BRACE_T:
        stack.push(t);
        break;
      default:
        break; //do nothing
      }
      break;

    case DOS_WHILE_S:
      if (t == Lossy::WHILE_T) {
        currState = WAIT_S;
      }
      break;

    case FORS_L_PAREN_S:
      if (t == Lossy::L_PAREN_T) {
        currState = FORS_INIT_S;
      }
      break;
    case FORS_INIT_S:
      if (t == Lossy::SINGLE_COLON_T) {
        currState = LAST_OTHER_S;
      }
      else if (t == Lossy::SEMICOLON_T) {
        currState = FORS_CONDITION_S;
      }
      break;
    case FORS_CONDITION_S:
      if (t == Lossy::SEMICOLON_T) {
        currState = LAST_OTHER_S;
      }
      break;

    case ELSE_OPTION_S:
      if (t == Lossy::IF_T) {
        stack.pop();
        stack.push(t);
        currState = L_PAREN_S;
      }
      else {
        if (t != Lossy::L_BRACE_T) {
          return lineNum;
        }
        currState = WAIT_S;
      }
      break;

    case L_PAREN_S:
      if (t == Lossy::L_PAREN_T) {
        currState = LAST_OTHER_S;
      }
      break;
    case LAST_OTHER_S:
      if (t == Lossy::R_PAREN_T) {
        currState = BRACE_S;
      }
      break;
    case BRACE_S:
      if (t != Lossy::L_BRACE_T) {
        return lineNum;
      }
      currState = WAIT_S;
      break;
    }
  }
  return -1;
}

#endif
