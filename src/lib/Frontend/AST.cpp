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
#include <set>
#include "Frontend/AST.h"

AST_ModuleDef* ASTParser::Parse(const std::vector<Token>& Tokens)
{
    std::vector<AST_FuncDef*> FuncDefList;
    int32_t TokenIndex = 0;
    bool bSuccess = false;
    do
    {
        bSuccess = false;
        AST_FuncDef* FuncDefAST = ParseFuncDef(Tokens, TokenIndex);
        if (FuncDefAST)
        {
            bSuccess = true;
            FuncDefList.push_back(FuncDefAST);
            continue;
        }
    } while (bSuccess && TokenIndex < Tokens.size());
    if (TokenIndex < Tokens.size())
    {
        return nullptr;
    }
    return new AST_ModuleDef(FuncDefList);
}
AST_FuncDef* ASTParser::ParseFuncDef(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    int32_t CurrentTokenIndex = TokenIndex;
    if (AST_Type* ReturnTypeAST = ParseType(Tokens, CurrentTokenIndex))
    {
        if (Match(Tokens, CurrentTokenIndex, ETokenType::Identifier))
        {
            std::string FuncName = Tokens[CurrentTokenIndex].TokenBuffer;
            CurrentTokenIndex++;
            bool bMatch = Match(Tokens, CurrentTokenIndex, ETokenType::LeftParen);
            CurrentTokenIndex++;
            bMatch &= Match(Tokens, CurrentTokenIndex, ETokenType::RightParen);
            CurrentTokenIndex++;
            bMatch &= Match(Tokens, CurrentTokenIndex, ETokenType::LeftBrace);
            CurrentTokenIndex++;
            if (bMatch)
            {
                AST_Statement* StatementAST = ParseStatement(Tokens, CurrentTokenIndex);
                if (StatementAST && Match(Tokens, CurrentTokenIndex, ETokenType::RightBrace))
                {
                    CurrentTokenIndex++;
                    TokenIndex = CurrentTokenIndex;
                    return new AST_FuncDef(FuncName, ReturnTypeAST, StatementAST);
                }
            }
        }
    }
    return nullptr;
}
AST_Assign* ASTParser::ParseAssign(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    if (Match(Tokens, TokenIndex, ETokenType::Identifier) &&
        Match(Tokens, TokenIndex + 1, ETokenType::Equal))
    {
        if (Match(Tokens, TokenIndex + 2, ETokenType::IntegerLiteral)
            && Match(Tokens, TokenIndex + 3, ETokenType::SemiColon))
        {
            int32_t CurrentTokenIndex = TokenIndex;
            TokenIndex += 4;
            return new AST_Assign(Tokens[CurrentTokenIndex].TokenBuffer, Tokens[CurrentTokenIndex + 2]);
        }
        else
        {
            int32_t NewStartTokenIndex = TokenIndex + 2;
            if (AST_Arithmetic* ArithmeticAST = ParseArithmetic(Tokens, NewStartTokenIndex))
            {
                if (Match(Tokens, NewStartTokenIndex, ETokenType::SemiColon))
                {
                    int32_t CurrentTokenIndex = TokenIndex;
                    NewStartTokenIndex++;
                    TokenIndex = NewStartTokenIndex;
                    return new AST_Assign(Tokens[CurrentTokenIndex].TokenBuffer, ArithmeticAST);
                }
            }
        }
    }
    return nullptr;
}
AST_Arithmetic* ASTParser::ParseArithmetic(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    if (Match(Tokens, TokenIndex, ETokenType::Identifier))
    {
        bool bPlus = Match(Tokens, TokenIndex + 1, ETokenType::Plus);
        bool bMinus = Match(Tokens, TokenIndex + 1, ETokenType::Minus);
        if (bPlus || bMinus)
        {
            if (Match(Tokens, TokenIndex + 2, ETokenType::Identifier))
            {
                int32_t CurrentTokenIndex = TokenIndex;
                TokenIndex += 3;
                EArithmeticType Type = bPlus ? EArithmeticType::Plus : EArithmeticType::Minus;
                return new AST_Arithmetic(Tokens[CurrentTokenIndex].TokenBuffer, Tokens[CurrentTokenIndex + 2].TokenBuffer, Type);
            }
        }
    }
    return nullptr;
}
AST_FuncCall* ASTParser::ParseFuncCall(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    int32_t CurrentTokenIndex = TokenIndex;
    if (Match(Tokens, CurrentTokenIndex, ETokenType::Identifier))
    {
        std::string FuncName = Tokens[CurrentTokenIndex].TokenBuffer;
        CurrentTokenIndex++;
        if (AST_ParameterCallList* ParameterCallList = ParseParameterCallList(Tokens, CurrentTokenIndex))
        {
            if (Match(Tokens, CurrentTokenIndex, ETokenType::SemiColon))
            {
                TokenIndex = CurrentTokenIndex + 1;
                return new AST_FuncCall(FuncName, ParameterCallList);
            }
        }
    }
    return nullptr;
}

AST_Type* ASTParser::ParseType(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    static std::set<std::string> SupportType = {"int", "float", "bool"};
    if (Match(Tokens, TokenIndex, ETokenType::Identifier) && SupportType.count(Tokens[TokenIndex].TokenBuffer) > 0)
    {
        int32_t CurrentTokenIndex = TokenIndex;
        TokenIndex++;
        return new AST_Type(Tokens[CurrentTokenIndex].TokenBuffer);
    }
    else
    {
        return nullptr;
    }
}

AST_Statement* ASTParser::ParseStatement(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    std::vector<AST*> Statements;
    int32_t CurrentTokenIndex = TokenIndex;
    bool bSuccess = false;
    do
    {
        bSuccess = false;
        AST* AssignAST = ParseAssign(Tokens, CurrentTokenIndex);
        if (AssignAST)
        {
            bSuccess = true;
            Statements.push_back(AssignAST);
            continue;
        }
        AST* VarDefAST = ParseVarDef(Tokens, CurrentTokenIndex);
        if (VarDefAST)
        {
            bSuccess = true;
            Statements.push_back(VarDefAST);
            continue;
        }
        AST* FuncCallAST = ParseFuncCall(Tokens, CurrentTokenIndex);
        if (FuncCallAST)
        {
            bSuccess = true;
            Statements.push_back(FuncCallAST);
            continue;
        }
        AST* ReturnAST = ParseReturn(Tokens, CurrentTokenIndex);
        if (ReturnAST)
        {
            bSuccess = true;
            Statements.push_back(ReturnAST);
            continue;
        }
    } while (bSuccess);
    TokenIndex = CurrentTokenIndex;
    return new AST_Statement(Statements);
}

AST_Return* ASTParser::ParseReturn(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    if (Match(Tokens, TokenIndex, ETokenType::Identifier, "return"))
    {
        bool bReturnLiteral = Match(Tokens, TokenIndex + 1, ETokenType::IntegerLiteral);
        bool bReturnVar = Match(Tokens, TokenIndex + 1, ETokenType::Identifier);
        bool bSemi = Match(Tokens, TokenIndex + 2, ETokenType::SemiColon);
        if ((bReturnLiteral || bReturnVar) && bSemi)
        {
            int32_t CurrentTokenIndex = TokenIndex;
            TokenIndex += 3;
            return new AST_Return(bReturnLiteral ? EReturnType::Literal : EReturnType::Variable, Tokens[CurrentTokenIndex + 1].TokenBuffer);
        }
    }
    return nullptr;
}

AST_VariableDef* ASTParser::ParseVarDef(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    int32_t CurrentTokenIndex = TokenIndex;
    if (AST_Type* TypeAST = ParseType(Tokens, CurrentTokenIndex))
    {
        if (Match(Tokens, CurrentTokenIndex, ETokenType::Identifier) && Match(Tokens, CurrentTokenIndex + 1, ETokenType::SemiColon))
        {
            std::string VarName = Tokens[CurrentTokenIndex].TokenBuffer;
            TokenIndex = CurrentTokenIndex + 2;
            return new AST_VariableDef(TypeAST, VarName);
        }
    }
    return nullptr;
}

AST_ParameterDefList* ASTParser::ParseParameterDefList(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    return nullptr;
}
AST_ParameterCallList* ASTParser::ParseParameterCallList(const std::vector<Token>& Tokens, int32_t& TokenIndex)
{
    std::vector<Token> ParameterCallList;
    int32_t CurrentTokenIndex = TokenIndex;
    if (Match(Tokens, CurrentTokenIndex, ETokenType::LeftParen))
    {
        CurrentTokenIndex++;
        while (!Match(Tokens, CurrentTokenIndex, ETokenType::RightParen))
        {
            bool bMatchIdentifier = Match(Tokens, CurrentTokenIndex, ETokenType::Identifier);
            bool bMatchComma = Match(Tokens, CurrentTokenIndex, ETokenType::Comma);
            if (bMatchIdentifier)
            {
                CurrentTokenIndex += 1;
                ParameterCallList.push_back(Tokens[CurrentTokenIndex]);
                if (bMatchComma)
                {
                    CurrentTokenIndex += 1;
                }
            }
        }
        CurrentTokenIndex += 1;
        TokenIndex = CurrentTokenIndex;
        return new AST_ParameterCallList(ParameterCallList);
    }
    return nullptr;
}

bool ASTParser::Match(const std::vector<Token>& Tokens, int32_t TokenIndex, ETokenType TokenType)
{
    return Tokens[TokenIndex].TokenType == TokenType;
}
bool ASTParser::Match(const std::vector<Token>& Tokens, int32_t TokenIndex, ETokenType TokenType, std::string TokenStr)
{
    return Tokens[TokenIndex].TokenType == TokenType && Tokens[TokenIndex].TokenBuffer == TokenStr;
}
