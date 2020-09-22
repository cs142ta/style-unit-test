#ifndef LOSSLESS_SCANNER_H_
#define LOSSLESS_SCANNER_H_

#include <iostream>
#include <vector>
#include <string>
#include "LosslessToken.h"

/**
 * Tokenizes the input. The types of tokens
 * are listed in LosslessToken.h.
 *
 */
std::vector<LosslessToken>
losslessScan(std::istream &in) {
  std::vector<LosslessToken> out;
  char c = '\0';
  char peekC = '\0';
  std::string currContents;
  int currLineNum = 1;
  LosslessToken::TokenType tokenToAdd = LosslessToken::NO_TYPE_T;
  bool addToContents = true;
  enum State {
    START_S,
    LINE_COMMENT_S,
    LINE_COMMENT_ESCAPE_S,
    BLOCK_COMMENT_S,
    END_BLOCK_INIT_S,
    ALPHA_S,
    STRING_LIT_S,
    STRING_ESC_S,
    CHAR_LIT_S,
    CHAR_ESC_S,
    PREPROCESSOR_S,
    PREPROCESSOR_ESC_S,
    PREPROC_BLOCK_COMMENT_S,
    PREPROC_END_BLOCK_INIT_S,
    NUMBER_S,
    RAW_STRING_S,
    END_RAW_STRING_INIT_S,
  } currState = START_S;
  while ((c = in.get()) != EOF) {
    tokenToAdd = LosslessToken::NO_TYPE_T;
    if (!iswspace(c) && addToContents) {
      currContents.push_back(c);
    }
    
    switch (currState) {
      case CHAR_LIT_S:
        switch (c) {
          case '\'':
          case '\n':
            currState = START_S;
            tokenToAdd = LosslessToken::CHAR_LIT_T;
            break;
          case '\\':
            currState = CHAR_ESC_S;
            break;
          default:
            break;//do nothing
        }
        break;

      case CHAR_ESC_S:
        if (c == '\n') {
          currState = START_S;
          tokenToAdd = LosslessToken::CHAR_LIT_T;
        } else {
          currState = CHAR_LIT_S;
        }
        break;

      case STRING_LIT_S:
        switch (c) {
          case '"':
          case '\n':
            currState = START_S;
            tokenToAdd = LosslessToken::STRING_LIT_T;
            break;
          case '\\':
            currState = STRING_ESC_S;
            break;
          default:
            break;//do nothing
        }
        break;

      case STRING_ESC_S:
        currState = STRING_LIT_S;
        break;

      case PREPROCESSOR_S:
        switch (c) {
          case '\\':
            currState = PREPROCESSOR_ESC_S;
            break;
          case '\n':
            tokenToAdd = LosslessToken::PREPROC_STATEMENT_T;
            currState = START_S;
            break;
          case '/':
            peekC = in.peek();
            if (peekC == '*') {
              in.get();
              currContents.push_back(peekC);
              addToContents = false;
              currState = PREPROC_BLOCK_COMMENT_S;
            }
            break;
          default:
            break;//do nothing
        }
        break;

      case PREPROCESSOR_ESC_S:
        currState = PREPROCESSOR_S;
        break;

      case START_S:
        switch (c) {
          case '"':
            currState = STRING_LIT_S;
            break;
          case '\'':
            currState = CHAR_LIT_S;
            break;
          case '#':
            currState = PREPROCESSOR_S;
            break;
          case '/':
            peekC = in.peek();
            if (peekC == '/') {
              in.get();
              currContents.clear();
              addToContents = false;
              currState = LINE_COMMENT_S;
            } else if (peekC == '*') {
              in.get();
              currContents.clear();
              addToContents = false;
              currState = BLOCK_COMMENT_S;
            } else {
              tokenToAdd = LosslessToken::FORWARD_SLASH_T;
            }
            break;

          case ':':
            peekC = in.peek();
            if (peekC == ':') {
              in.get();
              currContents.push_back(peekC);
              tokenToAdd = LosslessToken::DOUBLE_COLON_T;
            } else {
              tokenToAdd = LosslessToken::SINGLE_COLON_T;
            }
            break;
          case '=':
            peekC = in.peek();
            if (peekC == '=') {
              in.get();
              currContents.push_back(peekC);
              tokenToAdd = LosslessToken::DOUBLE_EQUALS_T;
            } else {
              tokenToAdd = LosslessToken::EQUALS_T;
            }
            break;
          case '<':
            peekC = in.peek();
            if (peekC == '<') {
              in.get();
              currContents.push_back(peekC);
              tokenToAdd = LosslessToken::DOUBLE_LT_T;
            } else {
              tokenToAdd = LosslessToken::LT_T;
            }
            break;
          case '>':
            peekC = in.peek();
            if (peekC == '>') {
              in.get();
              currContents.push_back(peekC);
              tokenToAdd = LosslessToken::DOUBLE_GT_T;
            } else {
              tokenToAdd = LosslessToken::GT_T;
            }
            break;
          case '&':
            peekC = in.peek();
            if (peekC == '&') {
              in.get();
              currContents.push_back(peekC);
              tokenToAdd = LosslessToken::DOUBLE_AND_T;
            } else {
              tokenToAdd = LosslessToken::AND_T;
            }
            break;
          case '|':
            peekC = in.peek();
            if (peekC == '|') {
              in.get();
              currContents.push_back(peekC);
              tokenToAdd = LosslessToken::DOUBLE_PIPE_T;
            } else {
              tokenToAdd = LosslessToken::PIPE_T;
            }
            break;

          case '+':
          case '-':
            peekC = in.peek();
            if (isdigit(peekC) || peekC == '.') {
              currState = NUMBER_S;
            } else {
              tokenToAdd = c == '-' ? LosslessToken::MINUS_T : LosslessToken::PLUS_T;
              currState = START_S;
            }
            break;

          case '{':
            tokenToAdd = LosslessToken::L_BRACE_T;
            break;
          case '}':
            tokenToAdd = LosslessToken::R_BRACE_T;
            break;
          case '(':
            tokenToAdd = LosslessToken::L_PAREN_T;
            break;
          case ')':
            tokenToAdd = LosslessToken::R_PAREN_T;
            break;
          case '[':
            tokenToAdd = LosslessToken::L_BRACKET_T;
            break;
          case ']':
            tokenToAdd = LosslessToken::R_BRACKET_T;
            break;
          case ';':
            tokenToAdd = LosslessToken::SEMICOLON_T;
            break;
          case ',':
            tokenToAdd = LosslessToken::COMMA_T;
            break;
          case '.':
            tokenToAdd = LosslessToken::DOT_T;
            break;
          case '!':
            tokenToAdd = LosslessToken::BANG_T;
            break;
          case '*':
            tokenToAdd = LosslessToken::STAR_T;
            break;
          case '%':
            tokenToAdd = LosslessToken::MOD_T;
            break;
          case '^':
            tokenToAdd = LosslessToken::CARET_T;
            break;
          case '~':
            tokenToAdd = LosslessToken::TWIDDLE_T;
            break;
          case '?':
            tokenToAdd = LosslessToken::QUESTION_T;
            break;

          default:
            peekC = in.peek();
            if (isalpha(c) || c == '_' || c == '$') {
              if (isalnum(peekC) || peekC == '_' || peekC == '$') {
                currState = ALPHA_S;
              } else if (peekC == '"') {
                if (c == 'R') {
                  currState = RAW_STRING_S;
                } else {
                  tokenToAdd = LosslessToken::WORD_T;
                }
              } else {
                tokenToAdd = LosslessToken::WORD_T;
              }
            } else if (isdigit(c)) {
              if (isdigit(peekC) || peekC == '.') {
                currState = NUMBER_S;
              } else {
                tokenToAdd = LosslessToken::NUMBER_T;
              }
            }
            break;
        }
        break;

      case ALPHA_S:
        peekC = in.peek();
        if (!isalnum(peekC) && peekC != '_' && peekC != '$') {
          if (currContents == "case") {
            tokenToAdd = LosslessToken::CASE_T;
          } else if (currContents == "catch") {
            tokenToAdd = LosslessToken::CATCH_T;
          } else if (currContents == "class") {
            tokenToAdd = LosslessToken::CLASS_T;
          } else if (currContents == "default") {
            tokenToAdd = LosslessToken::DEFAULT_T;
          } else if (currContents == "do") {
            tokenToAdd = LosslessToken::DO_T;
          } else if (currContents == "else") {
            tokenToAdd = LosslessToken::ELSE_T;
          } else if (currContents == "enum") {
            tokenToAdd = LosslessToken::ENUM_T;
          } else if (currContents == "for") {
            tokenToAdd = LosslessToken::FOR_T;
          } else if (currContents == "if") {
            tokenToAdd = LosslessToken::IF_T;
          } else if (currContents == "namespace") {
            tokenToAdd = LosslessToken::NAMESPACE_T;
          } else if (currContents == "private") {
            tokenToAdd = LosslessToken::PRIVATE_T;
          } else if (currContents == "protected") {
            tokenToAdd = LosslessToken::PROTECTED_T;
          } else if (currContents == "public") {
            tokenToAdd = LosslessToken::PUBLIC_T;
          } else if (currContents == "struct") {
            tokenToAdd = LosslessToken::STRUCT_T;
          } else if (currContents == "switch") {
            tokenToAdd = LosslessToken::SWITCH_T;
          } else if (currContents == "try") {
            tokenToAdd = LosslessToken::TRY_T;
          } else if (currContents == "union") {
            tokenToAdd = LosslessToken::UNION_T;
          } else if (currContents == "while") {
            tokenToAdd = LosslessToken::WHILE_T;
          } else if (currContents == "override") {
            tokenToAdd = LosslessToken::OVERRIDE_T;
          } else if (currContents == "final") {
            tokenToAdd = LosslessToken::FINAL_T;
          } else if (currContents == "const") {
            tokenToAdd = LosslessToken::CONST_T;
          } else if (currContents == "volatile") {
            tokenToAdd = LosslessToken::VOLATILE_T;
          } else if (currContents == "noexcept") {
            tokenToAdd = LosslessToken::NOEXCEPT_T;
          } else if (currContents == "using") {
            tokenToAdd = LosslessToken::USING_T;
          } else if (currContents == "typedef") {
            tokenToAdd = LosslessToken::TYPEDEF_T;
          } else {
            tokenToAdd = LosslessToken::WORD_T;
          }
          currState = START_S;
        }
        break;

      case LINE_COMMENT_S:
        if (c == '\\') {
          currState = LINE_COMMENT_ESCAPE_S;
        } else if (c == '\n') {
          addToContents = true;
          currState = START_S;
        }
        break;

      case LINE_COMMENT_ESCAPE_S:
        currState = LINE_COMMENT_S;
        break;

      case BLOCK_COMMENT_S:
        if (c == '*') {
          currState = END_BLOCK_INIT_S;
        }
        break;

      case END_BLOCK_INIT_S:
        if (c == '/') {
          addToContents = true;
          currState = START_S;
        } else if (c == '*') {
        } else {
          currState = BLOCK_COMMENT_S;
        }
        break;

      case PREPROC_BLOCK_COMMENT_S:
        if (c == '*') {
          currState = PREPROC_END_BLOCK_INIT_S;
        }
        break;

      case PREPROC_END_BLOCK_INIT_S:
        if (c == '/') {
          addToContents = true;
          currState = PREPROCESSOR_S;
        } else if (c == '*') {
        } else {
          currState = PREPROC_BLOCK_COMMENT_S;
        }
        break;

      case NUMBER_S:
        peekC = in.peek();
        if (!isdigit(peekC) && peekC != '.') {
          tokenToAdd = LosslessToken::NUMBER_T;
          currState = START_S;
        }
        break;

      case RAW_STRING_S:
        if (c == ')') {
          currState = END_RAW_STRING_INIT_S;
        }
        break;

      case END_RAW_STRING_INIT_S:
        if (c == '"') {
          tokenToAdd = LosslessToken::STRING_LIT_T;
          currState = START_S;
        } else if (c == ')') {
        } else {
          currState = RAW_STRING_S;
        }
        break;
    }

    if (tokenToAdd != LosslessToken::NO_TYPE_T) {
      out.emplace_back(tokenToAdd, currContents, currLineNum);
      currContents.clear();
    }
    if (c == '\n') {
      currLineNum++;
    }
  }
  tokenToAdd = LosslessToken::LosslessToken::EOF_T;
  out.emplace_back(LosslessToken::EOF_T, "", currLineNum);
  return out;
}

#endif //LOSSLESS_SCANNER_H_