#ifndef SINGLE_LETTER_VAR_WARNER_H_
#define SINGLE_LETTER_VAR_WARNER_H_

#include <vector>
#include "LosslessToken.h"

struct OkayLettersBox {
  int braceDepthIntroduced;
  std::string letters;

  explicit OkayLettersBox(int braceDepth)
    : braceDepthIntroduced(braceDepth) {}
};

bool isStartingRawString(const LosslessToken &curr, const LosslessToken &next);

bool isInOkays(char c, const std::vector<OkayLettersBox> &boxes) {
  for (const OkayLettersBox &box : boxes) {
    if (box.letters.find(c) != std::string::npos) {
      return true;
    }
  }
  return false;
}

int getFirstSingleLetterVarLine(const std::vector<LosslessToken> &tokens) {
  std::vector<OkayLettersBox> okayLettersBoxes;
  int braceDepth = 0;
  int parenDepth = 0;

  enum State {
    START_S,
    L_PAREN_WAIT_S,
    OKAY_GATHERING_S,
    CONDITION_S,
    STEP_S,
    PAREN_MATCHING_S,
    BRACE_WAITING_S,
  } currState = START_S;

  for (LosslessToken t : tokens) {
    switch (currState) {
      case START_S:
        switch (t.type) {
          case LosslessToken::FOR_T:
            okayLettersBoxes.emplace_back(braceDepth);
            currState = L_PAREN_WAIT_S;
            break;
          case LosslessToken::L_BRACE_T:
            braceDepth++;
            break;
          case LosslessToken::WORD_T:
            if (t.contents.size() == 1
                && !isInOkays(t.contents.front(), okayLettersBoxes)) {
              return t.lineNum;
            }
            break;
          case LosslessToken::R_BRACE_T:
            braceDepth--;
            if (!okayLettersBoxes.empty()
                && okayLettersBoxes.back().braceDepthIntroduced == braceDepth) {
              okayLettersBoxes.pop_back();
            }
            break;
          default:
            break;
        }
        break;
      case L_PAREN_WAIT_S:
        if (t.type == LosslessToken::L_PAREN_T) {
          currState = OKAY_GATHERING_S;
        }
        break;
      case OKAY_GATHERING_S:
        switch (t.type) {
          case LosslessToken::WORD_T:
            if (t.contents.size() == 1) {
              okayLettersBoxes.back().letters.push_back(t.contents.front());
            }
            break;
          case LosslessToken::SEMICOLON_T:
            currState = CONDITION_S;
            break;
          case LosslessToken::SINGLE_COLON_T:
            currState = STEP_S;
            break;
          default:
            break;
        }
        break;
      case CONDITION_S:
        if (t.type == LosslessToken::SEMICOLON_T) {
          currState = STEP_S;
        }
        break;
      case STEP_S:
        if (t.type == LosslessToken::L_PAREN_T) {
          parenDepth = 1;
          currState = PAREN_MATCHING_S;
        } else if (t.type == LosslessToken::R_PAREN_T) {
          currState = BRACE_WAITING_S;
        }
        break;
      case PAREN_MATCHING_S:
        if (t.type == LosslessToken::L_PAREN_T) {
          parenDepth++;
        } else if (t.type == LosslessToken::R_PAREN_T) {
          parenDepth--;
          if (parenDepth == 0) {
            currState = STEP_S;
          }
        }
        break;
      case BRACE_WAITING_S:
        if (t.type == LosslessToken::L_BRACE_T) {
          braceDepth++;
          currState = START_S;
        }
        break;
    }
  }


  return -1;
}

bool isStartingRawString(const LosslessToken &curr, const LosslessToken &next) {
  return curr.contents == "R"
        && next.type == LosslessToken::STRING_LIT_T
        && next.contents.find(R"("()") == 0;
}

#endif