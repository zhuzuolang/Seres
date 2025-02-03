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
#ifndef SERES_SRC_INCLUDE_FRONTEND_AST_H
#define SERES_SRC_INCLUDE_FRONTEND_AST_H
#include <vector>
#include "Core/Core.h"
#include "Lex.h"

struct AST {

  virtual ~AST() = default;
};

enum class EReturnType {
  None = 0,
  Literal,
  Variable,
};

struct AST_Return : public AST {
  AST_Return(EReturnType InReturnType, std::string InVariableName) :
      ReturnType(InReturnType),
      ReturnVariableName(InVariableName)
  {

  }
  EReturnType ReturnType;
  std::string ReturnVariableName;
};

struct AST_Type : public AST {
  AST_Type(std::string InTypeName) :
      TypeName(InTypeName)
  {

  }

  std::string TypeName;
};

struct AST_VariableDef : public AST {

  AST_VariableDef(AST_Type* InTypeAST, const std::string& InVarName)
      : TypeAST(InTypeAST),
        VariableName(InVarName)
  {

  }
  AST_Type* TypeAST = nullptr;
  std::string VariableName;
};

enum class EArithmeticType {
  None = 0,
  Plus,
  Minus,
};
struct AST_Arithmetic : public AST {
  AST_Arithmetic(std::string InLeftVarName, std::string InRightVarName, EArithmeticType InArithmeticType) :
      LeftVarName(InLeftVarName),
      RightVarName(InRightVarName),
      ArithmeticType(InArithmeticType)
  {

  }
  std::string LeftVarName;
  std::string RightVarName;
  EArithmeticType ArithmeticType;
};

struct AST_Assign : public AST {

  AST_Assign(std::string InAssignedVarName, Token InAssignLiteral) :
      bAssignLiteral(true),
      AssignedVarName(InAssignedVarName),
      AssignLiteral(InAssignLiteral)
  {

  }
  AST_Assign(std::string InAssignedVarName, AST_Arithmetic* InArithmeticAST) :
      bAssignLiteral(false),
      AssignLiteral(Token(ETokenType::None, "")),
      ArithmeticAST(InArithmeticAST)
  {

  }
  bool bAssignLiteral;
  std::string AssignedVarName;
  Token AssignLiteral;
  AST_Arithmetic* ArithmeticAST = nullptr;
};

struct AST_ParameterDefList : public AST {

  std::vector<AST_VariableDef*> ParameterDefList;
};

struct AST_ParameterCallList : public AST {

  AST_ParameterCallList(const std::vector<Token>& InParameterCallList) :
      ParameterCallList(InParameterCallList)
  {

  }
  std::vector<Token> ParameterCallList;
};

struct AST_FuncCall : public AST {
  AST_FuncCall(std::string InFuncName, AST_ParameterCallList* InParameterCallList) :
      FuncName(InFuncName),
      ParameterCallList(InParameterCallList)
  {

  }
  std::string FuncName;
  AST_ParameterCallList* ParameterCallList;
};

struct AST_Statement : public AST {
  AST_Statement(const std::vector<AST*>& InStatements) :
      Statements(InStatements)
  {

  }
  std::vector<AST*> Statements;
};

struct AST_FuncDef : public AST {
  AST_FuncDef(std::string InFuncName, AST_Type* InReturnTypeAST, AST_Statement* InStatementAST) :
      FuncName(InFuncName),
      ReturnTypeAST(InReturnTypeAST),
      StatementAST(InStatementAST)
  {

  }

  std::string FuncName;
  AST_Type* ReturnTypeAST = nullptr;
  AST_Statement* StatementAST = nullptr;
};

struct AST_ModuleDef : public AST {
  AST_ModuleDef(std::vector<AST_FuncDef*> InFuncDefList) :
      FuncDefList(InFuncDefList)
  {

  }
  std::vector<AST_FuncDef*> FuncDefList;
};

class ASTParser {

public:

    AST_ModuleDef* Parse(const std::vector<Token>& Tokens);

protected:
    AST_FuncDef* ParseFuncDef(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_Assign* ParseAssign(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_VariableDef* ParseVarDef(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_Statement* ParseStatement(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_Arithmetic* ParseArithmetic(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_FuncCall* ParseFuncCall(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_ParameterDefList* ParseParameterDefList(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_ParameterCallList* ParseParameterCallList(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_Type* ParseType(const std::vector<Token>& Tokens, int32_t& TokenIndex);
    AST_Return* ParseReturn(const std::vector<Token>& Tokens, int32_t& TokenIndex);

    bool Match(const std::vector<Token>& Tokens, int32_t TokenIndex, ETokenType TokenType);
    bool Match(const std::vector<Token>& Tokens, int32_t TokenIndex, ETokenType TokenType, std::string TokenStr);
};

#endif //SERES_SRC_INCLUDE_FRONTEND_AST_H
