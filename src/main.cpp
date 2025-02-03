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

#include <iostream>
#include "Frontend/Lex.h"
#include "Frontend/AST.h"
using namespace std;

int main(int argc, char** argv)
{
    Lex tokenizer;
    std::string Code = "int FuncTest(){return 3;} int main() { int b; int c; b = 100; c = 20; int a; a = b+c; printf(a); }";
    std::vector<Token> Result = tokenizer.LexFromBuffer(&Code[0], Code.length());
    ASTParser Parser;
    AST_ModuleDef* ModuleDefAST = dynamic_cast<AST_ModuleDef*>(Parser.Parse(Result));
    for (const auto& FuncDef: ModuleDefAST->FuncDefList)
    {
        cout << "FuncName:" <<FuncDef->FuncName<< endl;
    }
    //    for (const auto& Token: Result)
    //    {
    //        cout << "TokenType:" << int(Token.TokenType)<< "TokenStr:" << Token.TokenBuffer << endl;
    //    }
    return 0;
}