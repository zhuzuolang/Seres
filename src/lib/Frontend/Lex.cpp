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
#include "Core/Core.h"
#include "Frontend/Lex.h"

std::vector<Token> Lex::LexFromBuffer(char* Buf, int32_t Length)
{
    std::vector<Token> TokenList;
    int32_t CurrentIndex = 0;
    while (CurrentIndex < Length)
    {
        char Char = Buf[CurrentIndex];
        if (IsWhite(Char))
        {
            CurrentIndex++;
            continue;
        }
        else if (IsIdentifierStart(Char))
        {
            int32_t IdentifierStartIndex = CurrentIndex;
            CurrentIndex++;
            while (CurrentIndex < Length)
            {
                if (IsIdentifierBody(Buf[CurrentIndex]))
                {
                    CurrentIndex++;
                }
                else
                {
                    break;
                }
            }
            int32_t IdentifierEndIndex = CurrentIndex;
            std::string TokenStr(&Buf[IdentifierStartIndex], IdentifierEndIndex - IdentifierStartIndex);
            TokenList.emplace_back(ETokenType::Identifier, TokenStr);
            continue;
        }
        else if (IsSymbol(Char))
        {
            if (Char == '(')
            {
                TokenList.emplace_back(ETokenType::LeftParen, "");
            }
            else if (Char == ')')
            {
                TokenList.emplace_back(ETokenType::RightParen, "");
            }
            else if (Char == '+')
            {
                TokenList.emplace_back(ETokenType::Plus, "");
            }
            else if (Char == '-')
            {
                TokenList.emplace_back(ETokenType::Minus, "");
            }
            else if (Char == '=')
            {
                TokenList.emplace_back(ETokenType::Equal, "");
            }
            else if (Char == ';')
            {
                TokenList.emplace_back(ETokenType::SemiColon, "");
            }
            else
            {
                PLATFORM_BREAK();
            }
            CurrentIndex++;
        }
        else if (IsDigit(Char))
        {
            int32_t DigitStartIndex = CurrentIndex;
            CurrentIndex++;
            while (CurrentIndex < Length)
            {
                if (IsDigit(Buf[CurrentIndex]))
                {
                    CurrentIndex++;
                }
                else
                {
                    break;
                }
            }
            int32_t DigitEndIndex = CurrentIndex;
            std::string TokenStr(&Buf[DigitStartIndex], DigitEndIndex - DigitStartIndex);
            TokenList.emplace_back(ETokenType::IntegerLiteral, TokenStr);
            continue;
        }
    }
    return TokenList;
}
