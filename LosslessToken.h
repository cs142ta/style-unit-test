#ifndef LOSSLESS_TOKEN_H_
#define LOSSLESS_TOKEN_H_

#include <string>
#include <utility>

#include <sstream>
#include <iomanip>

struct LosslessToken {
  enum TokenType {
    CASE_T,
    CATCH_T,
    CLASS_T,
    DEFAULT_T,
    DO_T,
    ELSE_T,
    ENUM_T,
    FOR_T,
    IF_T,
    NAMESPACE_T,
    PRIVATE_T,
    PROTECTED_T,
    PUBLIC_T,
    STRUCT_T,
    SWITCH_T,
    TRY_T,
    UNION_T,
    WHILE_T,
    OVERRIDE_T,
    FINAL_T,
    CONST_T,
    VOLATILE_T,
    NOEXCEPT_T,
    USING_T,
    TYPEDEF_T,
    EOF_T,
    L_BRACE_T,
    R_BRACE_T,
    L_PAREN_T,
    R_PAREN_T,
    L_BRACKET_T,
    R_BRACKET_T,
    SEMICOLON_T,
    PREPROC_STATEMENT_T,
    SINGLE_COLON_T,
    DOUBLE_COLON_T,
    FORWARD_SLASH_T,
    WORD_T,
    NUMBER_T,
    STRING_LIT_T,
    CHAR_LIT_T,
    AND_T,
    DOUBLE_AND_T,
    PIPE_T,
    DOUBLE_PIPE_T,
    COMMA_T,
    EQUALS_T,
    DOUBLE_EQUALS_T,
    LT_T,
    GT_T,
    DOUBLE_LT_T,
    DOUBLE_GT_T,
    DOT_T,
    BANG_T,
    MINUS_T,
    PLUS_T,
    STAR_T,
    MOD_T,
    CARET_T,
    TWIDDLE_T,
    QUESTION_T,
    NO_TYPE_T
  } type;
  std::string contents;
  int lineNum;

  LosslessToken(TokenType t, std::string c, int l)
    : type(t), contents(std::move(c)), lineNum(l) {}

  std::string toString() const;
};

std::string losslessTokenTypeToStr(LosslessToken::TokenType t) {
  switch (t) {

    case LosslessToken::CASE_T:
      return "case";
    case LosslessToken::CATCH_T:
      return "catch";
    case LosslessToken::CLASS_T:
      return "class";
    case LosslessToken::DEFAULT_T:
      return "default";
    case LosslessToken::DO_T:
      return "do";
    case LosslessToken::ELSE_T:
      return "else";
    case LosslessToken::ENUM_T:
      return "enum";
    case LosslessToken::FOR_T:
      return "for";
    case LosslessToken::IF_T:
      return "if";
    case LosslessToken::NAMESPACE_T:
      return "namespace";
    case LosslessToken::PRIVATE_T:
      return "private";
    case LosslessToken::PROTECTED_T:
      return "protected";
    case LosslessToken::PUBLIC_T:
      return "public";
    case LosslessToken::STRUCT_T:
      return "struct";
    case LosslessToken::SWITCH_T:
      return "switch";
    case LosslessToken::TRY_T:
      return "try";
    case LosslessToken::UNION_T:
      return "union";
    case LosslessToken::WHILE_T:
      return "while";
    case LosslessToken::OVERRIDE_T:
      return "override";
    case LosslessToken::FINAL_T:
      return "final";
    case LosslessToken::CONST_T:
      return "const";
    case LosslessToken::VOLATILE_T:
      return "volatile";
    case LosslessToken::NOEXCEPT_T:
      return "noexcept";
    case LosslessToken::USING_T:
      return "using";
    case LosslessToken::TYPEDEF_T:
      return "typeder";
    case LosslessToken::EOF_T:
      return "<EOF>";
    case LosslessToken::L_BRACE_T:
      return "{";
    case LosslessToken::R_BRACE_T:
      return "}";
    case LosslessToken::L_PAREN_T:
      return "(";
    case LosslessToken::R_PAREN_T:
      return ")";
    case LosslessToken::L_BRACKET_T:
      return "[";
    case LosslessToken::R_BRACKET_T:
      return "]";
    case LosslessToken::SEMICOLON_T:
      return ";";
    case LosslessToken::PREPROC_STATEMENT_T:
      return "#-->";
    case LosslessToken::SINGLE_COLON_T:
      return ":";
    case LosslessToken::DOUBLE_COLON_T:
      return "::";
    case LosslessToken::FORWARD_SLASH_T:
      return "/";
    case LosslessToken::WORD_T:
      return "w->";
    case LosslessToken::NUMBER_T:
      return "x->";
    case LosslessToken::STRING_LIT_T:
      return "\"->";
    case LosslessToken::CHAR_LIT_T:
      return "'->";
    case LosslessToken::AND_T:
      return "&";
    case LosslessToken::DOUBLE_AND_T:
      return "&&";
    case LosslessToken::PIPE_T:
      return "|";
    case LosslessToken::DOUBLE_PIPE_T:
      return "||";
    case LosslessToken::COMMA_T:
      return ",";
    case LosslessToken::EQUALS_T:
      return "=";
    case LosslessToken::DOUBLE_EQUALS_T:
      return "==";
    case LosslessToken::LT_T:
      return "<";
    case LosslessToken::GT_T:
      return ">";
    case LosslessToken::DOUBLE_LT_T:
      return "<<";
    case LosslessToken::DOUBLE_GT_T:
      return ">>";
    case LosslessToken::DOT_T:
      return ".";
    case LosslessToken::BANG_T:
      return "!";
    case LosslessToken::MINUS_T:
      return "-";
    case LosslessToken::PLUS_T:
      return "+";
    case LosslessToken::STAR_T:
      return "*";
    case LosslessToken::MOD_T:
      return "%";
    case LosslessToken::CARET_T:
      return "^";
    case LosslessToken::TWIDDLE_T:
      return "~";
    case LosslessToken::QUESTION_T:
      return "?";
    case LosslessToken::NO_TYPE_T:
      return "SCARY!!";
  }
}



std::string LosslessToken::toString() const {
  std::ostringstream out;
  out << std::setw(3) << lineNum << ":";
  out << contents;

  return out.str();
}


#endif //LOSSLESS_TOKEN_H_
