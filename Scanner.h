#ifndef SCANNER_H_
#define SCANNER_H_

#include <iostream>
#include <vector>
#include <string>
#include "Token.h"

#define addToken(t)                                     \
  if ((t) != Lossy::OTHER_STUFF_T ||                           \
      out.empty() || out.back() != (Lossy::OTHER_STUFF_T)) {   \
    out.push_back(t);                                   \
  }

/**
 * Removes nested parens and whitespace
 * that is in the middle of lines
 * (Things that don't affect indentation, bracing,
 *   or the understanding thereof)
 *
 */
std::vector<Lossy::Token>
removeExcess(const std::vector<Lossy::Token> &in) {
  std::vector<Lossy::Token> out;
  bool doRemove = false;
  int parenDepth = 0;
  bool doAdd = true;
  Lossy::Token toAdd = Lossy::OTHER_STUFF_T;
  for (Lossy::Token t: in) {
    doAdd = true;
    switch (t) {
      case Lossy::SINGLE_SPACE_T:
      case Lossy::TAB_T:
        if (doRemove) {
          doAdd = false;
        }
        break;
      case Lossy::NEWLINE_T:
        doRemove = false;
        break;
      case Lossy::L_PAREN_T:
        if (parenDepth++) {
          doAdd = false;
        }
        break;
      case Lossy::R_PAREN_T:
        if (--parenDepth) {
          doAdd = false;
        }
        break;
      default:
        doRemove = true;
        break;
    }
    if (doAdd) {
      addToken(t);
    }
  }
  return out;
}

/**
 * Tokenizes the input. The types of tokens
 * are listed in Token.h. Of particular note,
 * spaces, tabs, and newlines are tokens,
 * and many complex things become the token 'Other Stuff'
 *
 */
std::vector<Lossy::Token>
scan(std::istream &in) {
  std::vector<Lossy::Token> out;
  char prevC = '\0';
  char c = '\0';
  char peekC = '\0';
  std::string currWord;
  enum State {
    START_S,
    LINE_COMMENT_S,
    LINE_COMMENT_ESCAPE_S,
    BLOCK_COMMENT_S,
    END_BLOCK_INIT_S,
    ALPHA_S,
    CRLF_MID_S,
    STRING_LIT_S,
    STRING_ESC_S,
    CHAR_LIT_S,
    CHAR_ESC_S,
    PREPROCESSOR_S,
    PREPROCESSOR_ESC_S,
    PREPROC_BLOCK_COMMENT_S,
    PREPROC_END_BLOCK_INIT_S,
    RAW_STRING_S,
    END_RAW_STRING_INIT_S,
  } currState = START_S;
  while ((c = in.get()) != EOF) {
    //std::cout << "State: " << currState << '\n';
    switch (currState) {
      case CHAR_LIT_S:
        switch (c) {
          case '\'':
            currState = START_S;
            break;
          case '\n':
            out.push_back(Lossy::NEWLINE_T);
            currState = START_S;
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
          out.push_back(Lossy::NEWLINE_T);
          currState = START_S;
        } else {
          currState = CHAR_LIT_S;
        }
        break;

      case STRING_LIT_S:
        switch (c) {
          case '"':
            currState = START_S;
            break;
          case '\n':
            out.push_back(Lossy::NEWLINE_T);
            currState = START_S;
            break;
          case '\\':
            currState = STRING_ESC_S;
            break;
          default:
            break;//do nothing
        }
        break;

      case STRING_ESC_S:
        if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        }
        currState = STRING_LIT_S;
        break;

      case PREPROCESSOR_S:
        switch (c) {
          case '\\':
            currState = PREPROCESSOR_ESC_S;
            break;
          case '\n':
            out.push_back(Lossy::NEWLINE_T);
            currState = START_S;
            break;
          case '/':
            peekC = in.peek();
            if (peekC == '*') {
              in.get();
              currState = PREPROC_BLOCK_COMMENT_S;
            }
            break;
          default:
            break;//do nothing
        }
        break;

      case PREPROCESSOR_ESC_S:
        if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        }
        currState = PREPROCESSOR_S;
        break;

      case START_S:
        switch (c) {
          case '"':
            addToken(Lossy::OTHER_STUFF_T);
            if (prevC == 'R' && in.peek() == '(') {
              currState = RAW_STRING_S;
            } else {
              currState = STRING_LIT_S;
            }
            break;
          case '\'':
            addToken(Lossy::OTHER_STUFF_T);
            currState = CHAR_LIT_S;
            break;
          case '#':
            out.push_back(Lossy::OCTOTHORPE_T);
            addToken(Lossy::OTHER_STUFF_T);
            currState = PREPROCESSOR_S;
            break;
          case '/':
            peekC = in.peek();
            if (peekC == '/') {
              in.get();
              currState = LINE_COMMENT_S;
            } else if (peekC == '*') {
              in.get();
              currState = BLOCK_COMMENT_S;
            } else {
              addToken(Lossy::OTHER_STUFF_T);
            }
            break;

          case ':':
            peekC = in.peek();
            if (peekC == ':') {
              in.get();
              addToken(Lossy::OTHER_STUFF_T);
            } else {
              out.push_back(Lossy::SINGLE_COLON_T);
            }
            break;

          case '\r':
            currState = CRLF_MID_S;
            break;

          case '{':
            out.push_back(Lossy::L_BRACE_T);
            break;
          case '}':
            out.push_back(Lossy::R_BRACE_T);
            break;
          case '(':
            out.push_back(Lossy::L_PAREN_T);
            break;
          case ')':
            out.push_back(Lossy::R_PAREN_T);
            break;
          case ';':
            out.push_back(Lossy::SEMICOLON_T);
            break;
          case ' ':
            out.push_back(Lossy::SINGLE_SPACE_T);
            break;
          case '\t':
            out.push_back(Lossy::TAB_T);
            break;
          case '\n':
            out.push_back(Lossy::NEWLINE_T);
            break;

          default:
            if (isalpha(c) &&
                (!isalnum(prevC) && prevC != '_') &&
                isalpha(in.peek())) {
              currWord.clear();
              currWord.push_back(c);
              currState = ALPHA_S;
            } else {
              addToken(Lossy::OTHER_STUFF_T);
            }
            break;
        }
        break;

      case CRLF_MID_S:
        if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        } else {
          addToken(Lossy::OTHER_STUFF_T);
        }
        currState = START_S;
        break;

      case ALPHA_S:
        currWord.push_back(c);
        if (!isalpha(in.peek())) {
          if (currWord == "case") {
            out.push_back(Lossy::CASE_T);
          } else if (currWord == "catch") {
            out.push_back(Lossy::CATCH_T);
          } else if (currWord == "class") {
            out.push_back(Lossy::CLASS_T);
          } else if (currWord == "default") {
            out.push_back(Lossy::DEFAULT_T);
          } else if (currWord == "do") {
            out.push_back(Lossy::DO_T);
          } else if (currWord == "else") {
            out.push_back(Lossy::ELSE_T);
          } else if (currWord == "enum") {
            out.push_back(Lossy::ENUM_T);
          } else if (currWord == "for") {
            out.push_back(Lossy::FOR_T);
          } else if (currWord == "if") {
            out.push_back(Lossy::IF_T);
          } else if (currWord == "namespace") {
            out.push_back(Lossy::NAMESPACE_T);
          } else if (currWord == "private") {
            out.push_back(Lossy::PRIVATE_T);
          } else if (currWord == "protected") {
            out.push_back(Lossy::PROTECTED_T);
          } else if (currWord == "public") {
            out.push_back(Lossy::PUBLIC_T);
          } else if (currWord == "struct") {
            out.push_back(Lossy::STRUCT_T);
          } else if (currWord == "switch") {
            out.push_back(Lossy::SWITCH_T);
          } else if (currWord == "try") {
            out.push_back(Lossy::TRY_T);
          } else if (currWord == "union") {
            out.push_back(Lossy::UNION_T);
          } else if (currWord == "while") {
            out.push_back(Lossy::WHILE_T);
          } else if (currWord == "override") {
            out.push_back(Lossy::OVERRIDE_T);
          } else if (currWord == "final") {
            out.push_back(Lossy::FINAL_T);
          } else if (currWord == "const") {
            out.push_back(Lossy::CONST_T);
          } else if (currWord == "constexpr") {
            out.push_back(Lossy::CONSTEXPR_T);
          } else if (currWord == "volatile") {
            out.push_back(Lossy::VOLATILE_T);
          } else if (currWord == "noexcept") {
            out.push_back(Lossy::NOEXCEPT_T);
          } else if (currWord == "using") {
            out.push_back(Lossy::USING_T);
          } else if (currWord == "typedef") {
            out.push_back(Lossy::TYPEDEF_T);
          } else {
            addToken(Lossy::OTHER_STUFF_T);
          }
          currState = START_S;
        }
        break;

      case LINE_COMMENT_S:
        if (c == '\\') {
          currState = LINE_COMMENT_ESCAPE_S;
        } else if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
          currState = START_S;
        }
        break;

      case LINE_COMMENT_ESCAPE_S:
        if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        }
        currState = LINE_COMMENT_S;
        break;

      case BLOCK_COMMENT_S:
        if (c == '*') {
          currState = END_BLOCK_INIT_S;
        } else if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        }
        break;

      case END_BLOCK_INIT_S:
        if (c == '/') {
          currState = START_S;
        } else if (c == '*') {
        } else {
          if (c == '\n') {
            out.push_back(Lossy::NEWLINE_T);
          }
          currState = BLOCK_COMMENT_S;
        }
        break;

      case PREPROC_BLOCK_COMMENT_S:
        if (c == '*') {
          currState = PREPROC_END_BLOCK_INIT_S;
        } else if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        }
        break;

      case PREPROC_END_BLOCK_INIT_S:
        if (c == '/') {
          currState = PREPROCESSOR_S;
        } else if (c == '*') {
        } else {
          if (c == '\n') {
            out.push_back(Lossy::NEWLINE_T);
          }
          currState = PREPROC_BLOCK_COMMENT_S;
        }
        break;

      case RAW_STRING_S:
        if (c == ')') {
          currState = END_RAW_STRING_INIT_S;
        } else if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
        }
        break;

      case END_RAW_STRING_INIT_S:
        if (c == '"') {
          currState = START_S;
        } else if (c == '\n') {
          out.push_back(Lossy::NEWLINE_T);
          currState = RAW_STRING_S;
        } else if (c == ')') {
        } else {
          currState = RAW_STRING_S;
        }
        break;
    }
    prevC = c;
  }
  out.push_back(Lossy::EOF_T);

  return removeExcess(out);
}

#endif //SCANNER_H_