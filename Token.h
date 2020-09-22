#ifndef TOKEN_H_
#define TOKEN_H_

//#define DEBUG

namespace Lossy {
  enum Token {
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
    CONSTEXPR_T,
    VOLATILE_T,
    NOEXCEPT_T,
    USING_T,
    TYPEDEF_T,
    EOF_T,
    L_BRACE_T,
    R_BRACE_T,
    L_PAREN_T,
    R_PAREN_T,
    SEMICOLON_T,
    OCTOTHORPE_T,
    SINGLE_COLON_T,
    TAB_T,
    SINGLE_SPACE_T,
    NEWLINE_T,
    OTHER_STUFF_T,
  };
};

bool tokenIsws(Lossy::Token t) {
  return t == Lossy::SINGLE_SPACE_T
         || t == Lossy::TAB_T
         || t == Lossy::NEWLINE_T;
}

bool tokenIshws(Lossy::Token t) {
  return t == Lossy::SINGLE_SPACE_T
         || t == Lossy::TAB_T;
}

bool isBraceInLineScopeOpen(const std::vector<Lossy::Token>& line) {
  return line.size() == 1 && line.front() == Lossy::L_BRACE_T;
}

bool isAFunctionHeadLine(const std::vector<Lossy::Token>& line) {
  std::vector<Lossy::Token>::const_reverse_iterator rit
          = line.crbegin();
  rit++; //skip over the brace or semicolon (for forward-declared funcs)
  bool foundDefault = false;
  for (; rit != line.crend(); rit++) {
    if (!tokenIsws(*rit)) {
      if (*rit == Lossy::R_PAREN_T) {
        return true;
      }
      else if (*rit == Lossy::DEFAULT_T) {
        foundDefault = true;
      }
      else if (!foundDefault
               && *rit != Lossy::OVERRIDE_T
               && *rit != Lossy::FINAL_T
               && *rit != Lossy::CONST_T
               && *rit != Lossy::CONSTEXPR_T
               && *rit != Lossy::VOLATILE_T
               && *rit != Lossy::NOEXCEPT_T) {
        return false;
      }
    }
  }
  return false;
}
bool isBraceInLineFunctionOpen(const std::vector<Lossy::Token>& line) {
  if (line.back() != Lossy::L_BRACE_T) {
    return false;
  }
  return isAFunctionHeadLine(line);
}

bool isBraceInLineOpeningIndentLevel(const std::vector<Lossy::Token>& line) {
  return isBraceInLineScopeOpen(line)
         || isBraceInLineFunctionOpen(line);
}

#ifdef DEBUG
const char* tokenToStr(Lossy::Token t) {
  switch (t) {
    case        Lossy::CASE_T:
      return "case";
    case        Lossy::CATCH_T:
      return "catch";
    case        Lossy::CLASS_T:
      return "class";
    case        Lossy::DEFAULT_T:
      return "default";
    case        Lossy::DO_T:
      return "do";
    case        Lossy::ELSE_T:
      return "else";
    case        Lossy::ENUM_T:
      return "enum";
    case        Lossy::FOR_T:
      return "for";
    case        Lossy::IF_T:
      return "if";
    case        Lossy::NAMESPACE_T:
      return "namespace";
    case        Lossy::PRIVATE_T:
      return "private";
    case        Lossy::PROTECTED_T:
      return "protected";
    case        Lossy::PUBLIC_T:
      return "public";
    case        Lossy::STRUCT_T:
      return "struct";
    case        Lossy::SWITCH_T:
      return "switch";
    case        Lossy::TRY_T:
      return "try";
    case        Lossy::UNION_T:
      return "union";
    case        Lossy::WHILE_T:
      return "while";
    case        Lossy::OVERRIDE_T:
      return "override";
    case        Lossy::FINAL_T:
      return "final";
    case        Lossy::CONST_T:
      return "const";
    case        Lossy::CONSTEXPR_T:
      return "constexpr";
    case        Lossy::VOLATILE_T:
      return "volatile";
    case        Lossy::NOEXCEPT_T:
      return "noexcept";
    case        Lossy::USING_T:
      return "using";
    case        Lossy::TYPEDEF_T:
      return "typedef";
    case        Lossy::EOF_T:
      return "!!EOF!!\n";
    case        Lossy::L_BRACE_T:
      return "{";
    case        Lossy::R_BRACE_T:
      return "}";
    case        Lossy::L_PAREN_T:
      return "(";
    case        Lossy::R_PAREN_T:
      return ")";
    case        Lossy::SEMICOLON_T:
      return ";";
    case        Lossy::SINGLE_SPACE_T:
      return "_";
    case        Lossy::TAB_T:
      return ">";
    case        Lossy::SINGLE_COLON_T:
      return ":";
    case        Lossy::OCTOTHORPE_T:
      return "#";
    case        Lossy::OTHER_STUFF_T:
      return "%";
    case        Lossy::NEWLINE_T:
      return "\\n\n";
    default:
      return "!!ERROR!!";
  }
}
#endif

#endif //TOKEN_H_
