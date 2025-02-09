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
#include <unordered_map>
#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "Core/Core.h"
#include "Lex.h"

struct CodeGenContext {
  llvm::LLVMContext* Context;
  llvm::Module* Module;
  llvm::IRBuilder<>* IRBuilder;
  llvm::StringMap<llvm::AllocaInst*> SymbolTables;
};

struct AST {

  virtual ~AST() = default;

  virtual void GenCode(CodeGenContext& CodeGenContext)
  {

  }
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
  void GenCode(CodeGenContext& InCodeGenContext) override
  {
      llvm::Value *retValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*InCodeGenContext.Context), 0);
      InCodeGenContext.IRBuilder->CreateRet(retValue);
  }
  EReturnType ReturnType;
  std::string ReturnVariableName;
};

struct AST_Type : public AST {
  AST_Type(std::string InTypeName) :
      TypeName(InTypeName)
  {

  }
  llvm::Type* GenLLVMType(CodeGenContext& CodeGenContext)
  {
      if (TypeName == "int")
      {
          return llvm::Type::getInt32Ty(*CodeGenContext.Context);
      }
      else if (TypeName == "float")
      {
          return llvm::Type::getFloatTy(*CodeGenContext.Context);
      }
      else
      {
          check(false);
          return nullptr;
      }
  }
  std::string TypeName;
};

struct AST_VariableDef : public AST {

  AST_VariableDef(AST_Type* InTypeAST, const std::string& InVarName)
      : TypeAST(InTypeAST),
        VariableName(InVarName)
  {

  }

  void GenCode(CodeGenContext& InCodeGenContext) override
  {
      llvm::AllocaInst* VariableDefInst = InCodeGenContext.IRBuilder->CreateAlloca(TypeAST->GenLLVMType(InCodeGenContext), nullptr, VariableName);
      InCodeGenContext.SymbolTables[VariableName] = VariableDefInst;
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

  llvm::Value* GenValue(CodeGenContext& InCodeGenContext)
  {
      llvm::AllocaInst* LeftInst = InCodeGenContext.SymbolTables.lookup(LeftVarName);
      llvm::AllocaInst* RightInst = InCodeGenContext.SymbolTables.lookup(RightVarName);
      llvm::Value* LeftVar = InCodeGenContext.IRBuilder->CreateLoad(LeftInst->getAllocatedType(), LeftInst);
      llvm::Value* RightVar = InCodeGenContext.IRBuilder->CreateLoad(RightInst->getAllocatedType(), RightInst);
      if (ArithmeticType == EArithmeticType::Plus)
      {
          return InCodeGenContext.IRBuilder->CreateAdd(LeftVar, RightVar);
      }
      else if (ArithmeticType == EArithmeticType::Minus)
      {
          //todo
          return InCodeGenContext.IRBuilder->CreateAdd(LeftVar, RightVar);
      }
      else
      {
          check(false);
      }
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
      AssignedVarName(InAssignedVarName),
      AssignLiteral(Token(ETokenType::None, "")),
      ArithmeticAST(InArithmeticAST)
  {

  }

  void GenCode(CodeGenContext& InCodeGenContext) override
  {
      if (bAssignLiteral)
      {
          if (AssignLiteral.TokenType == ETokenType::IntegerLiteral)
          {
              llvm::AllocaInst* AllocaInst = InCodeGenContext.SymbolTables.lookup(AssignedVarName);
              llvm::Value* AssignValue = llvm::ConstantInt::get(AllocaInst->getOperand(0)->getType(), std::atoi(AssignLiteral.TokenBuffer.c_str()));
              InCodeGenContext.IRBuilder->CreateStore(AssignValue, AllocaInst);
          }
      }
      else
      {
          llvm::AllocaInst* AllocaInst = InCodeGenContext.SymbolTables.lookup(AssignedVarName);
          llvm::Value* AssignValue = ArithmeticAST->GenValue(InCodeGenContext);
          InCodeGenContext.IRBuilder->CreateStore(AssignValue, AllocaInst);
      }
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

  std::vector<llvm::Value*> GenParameterValueList(CodeGenContext& InCodeGenContext)
  {
      std::vector<llvm::Value*> Result;
      for (auto& Parameter: ParameterCallList)
      {
          if (Parameter.TokenType == ETokenType::Identifier)
          {
              llvm::AllocaInst* AllocaInst = InCodeGenContext.SymbolTables.lookup(Parameter.TokenBuffer);
              llvm::Value* Var = InCodeGenContext.IRBuilder->CreateLoad(AllocaInst->getAllocatedType(), AllocaInst);
              Result.push_back(Var);
          }
          else
          {
              check(false);
          }
      }
      return Result;
  }
  std::vector<Token> ParameterCallList;
};

struct AST_FuncCall : public AST {
  AST_FuncCall(std::string InFuncName, AST_ParameterCallList* InParameterCallList) :
      FuncName(InFuncName),
      ParameterCallList(InParameterCallList)
  {

  }
  void GenCode(CodeGenContext& InCodeGenContext) override
  {
      llvm::FunctionType* printfType = llvm::FunctionType::get(
          InCodeGenContext.IRBuilder->getInt32Ty(),
          {InCodeGenContext.IRBuilder->getPtrTy()},
          true
      );
      llvm::Function* printfFunc = llvm::Function::Create(
          printfType,
          llvm::Function::ExternalLinkage,
          "printf",
          InCodeGenContext.Module
      );
      std::vector<llvm::Value*> FormatedParameterList = ParameterCallList->GenParameterValueList(InCodeGenContext);
      std::string FormatStr;
      for (int i = 0; i < FormatedParameterList.size(); i++)
      {
          FormatStr += "%d ";
      }
      FormatStr += "\n";
      std::vector<llvm::Value*> CallParameterList;
      llvm::Value* str = InCodeGenContext.IRBuilder->CreateGlobalStringPtr(FormatStr, "str");
      CallParameterList.push_back(str);
      for (auto&& Value: FormatedParameterList)
      {
          CallParameterList.push_back(Value);
      }
      InCodeGenContext.IRBuilder->CreateCall(printfFunc, CallParameterList);
  }

  std::string FuncName;
  AST_ParameterCallList* ParameterCallList;
};

struct AST_Statement : public AST {
  AST_Statement(const std::vector<AST*>& InStatements) :
      Statements(InStatements)
  {

  }
  void GenCode(CodeGenContext& InCodeGenContext) override
  {
      for (AST* Ast: Statements)
      {
          Ast->GenCode(InCodeGenContext);
      }
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

  void GenCode(CodeGenContext& InCodeGenContext) override
  {
      llvm::IRBuilder<> IRBuilder(*InCodeGenContext.Context);
      CodeGenContext NewCodeGenContext;
      NewCodeGenContext.Context = InCodeGenContext.Context;
      NewCodeGenContext.Module = InCodeGenContext.Module;
      NewCodeGenContext.IRBuilder = &IRBuilder;

      llvm::Type* ReturnType = ReturnTypeAST->GenLLVMType(NewCodeGenContext);
      llvm::FunctionType* FuncType = llvm::FunctionType::get(
          ReturnType,
          {},
          false
      );
      llvm::Function* Func = llvm::Function::Create(
          FuncType,
          llvm::Function::ExternalLinkage,
          FuncName,
          NewCodeGenContext.Module
      );
      llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*NewCodeGenContext.Context, "entry", Func);
      NewCodeGenContext.IRBuilder->SetInsertPoint(entryBB);
      StatementAST->GenCode(NewCodeGenContext);
      if (llvm::verifyFunction(*Func, &llvm::errs())) {
          llvm::errs() << "函数验证失败！\n";
          check(false);
      }
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

  CodeGenContext StartGenCode(llvm::LLVMContext& Context)
  {
      CodeGenContext NewCodeGenContext;
      llvm::Module* CurrentModule = new llvm::Module("TestModule", Context);
      NewCodeGenContext.Context = &Context;
      NewCodeGenContext.Module = CurrentModule;
      for (AST_FuncDef* AstFuncDef: FuncDefList)
      {
          AstFuncDef->GenCode(NewCodeGenContext);
      }
      if (llvm::verifyModule(*NewCodeGenContext.Module, &llvm::errs())) {
          llvm::errs() << "模块验证失败！\n";
          check(false);
      }
      return NewCodeGenContext;

      //      std::string temp_str;
      //      llvm::raw_string_ostream rso(temp_str);
      //      NewCodeGenContext.Module->print(rso, nullptr);
      //      std::string s = rso.str();
      //      std::cout << s << std::endl;
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
