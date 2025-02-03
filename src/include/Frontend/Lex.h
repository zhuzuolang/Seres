/*----------------------------------------------------------------------
    Copyright 2025/1/26 zhuzuolang

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
----------------------------------------------------------------------
*/
#ifndef SERES_SRC_INCLUDE_FRONTEND_LEX_H
#define SERES_SRC_INCLUDE_FRONTEND_LEX_H
#include <string>
#include <vector>

enum class ETokenType {
  None = 0,
  Identifier,
  Int,
  SingleLineStringLiteral,
  IntegerLiteral,
  LeftParen,// 左圆括号
  RightParen,// 右圆括号
  LeftBrace,// 左大括号
  RightBrace,// 右大括号
  Minus,// 减号
  Plus,// 加号
  Equal,// 等号
  SemiColon,//分号
  Comma,//逗号
  EndOfFile,
  ErrorToken,
};

struct Token {
  Token(ETokenType InTokenType, std::string InTokenBuffer)
      : TokenType(InTokenType),
        TokenBuffer(InTokenBuffer)
  {

  }
  ETokenType TokenType = ETokenType::None;
  std::string TokenBuffer;
};

class Lex {

public:

    Lex() = default;

    std::vector<Token> LexFromBuffer(char* Buf, int32_t Length);

private:

    inline bool IsWhite(char c)
    {
        return (' ' == c) || ('\n' == c) ||
            ('\r' == c) || ('\t' == c) ||
            ('\b' == c) || ('\v' == c) ||
            ('\f' == c);
    }

    inline bool IsDigit(char ucs)
    {
        return ucs <= '9' && ucs >= '0';
    }

    inline bool IsNonDigit(char ucs)
    {
        return ucs <= '9' && ucs > '0';
    }

    inline bool IsIdentifierStart(char c)
    {
        return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    inline bool IsIdentifierBody(char c)
    {
        return IsIdentifierStart(c) || (c >= '0' && c <= '9');
    }

    inline bool IsEndOfLine(char c)
    {
        return c == '\n';
    }

    inline bool IsSymbol(char c)
    {
        return '~' == c ||
            '!' == c ||
            '@' == c ||
            '#' == c ||
            '$' == c ||
            '%' == c ||
            '^' == c ||
            '&' == c ||
            '*' == c ||
            '(' == c ||
            ')' == c ||
            '-' == c ||
            '+' == c ||
            '=' == c ||
            '[' == c ||
            ']' == c ||
            '{' == c ||
            '}' == c ||
            '\\' == c ||
            '|' == c ||
            '\'' == c ||
            '\"' == c ||
            ';' == c ||
            ':' == c ||
            '<' == c ||
            '>' == c ||
            '/' == c ||
            '?' == c ||
            ',' == c ||
            '.' == c;
    }
private:
};

#endif //SERES_SRC_INCLUDE_FRONTEND_LEX_H
