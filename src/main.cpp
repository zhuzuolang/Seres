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

#include "llvm/ADT/MapVector.h"
#include "llvm/BinaryFormat/Magic.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/Frontend/Offloading/OffloadWrapper.h"
#include "llvm/Frontend/Offloading/Utility.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/LTO/LTO.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/ArchiveWriter.h"
#include "llvm/Object/Binary.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/IRObjectFile.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Object/OffloadBinary.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Remarks/HotnessThresholdParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileOutputBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Parallel.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"

#include "Frontend/Lex.h"
#include "Frontend/AST.h"
using namespace std;

int main(int argc, char** argv)
{
    Lex tokenizer;
    std::string Code = "int main() { int b; int c; b = 100; c = 20; int a; a = b+c; printf(a); return 0;}";
    std::vector<Token> Result = tokenizer.LexFromBuffer(&Code[0], Code.length());
    //    for (const auto& Token: Result)
    //    {
    //        cout << "Token:" << int(Token.TokenType) << " " << Token.TokenBuffer << endl;
    //    }

    ASTParser Parser;
    AST_ModuleDef* ModuleDefAST = dynamic_cast<AST_ModuleDef*>(Parser.Parse(Result));
    for (const auto& FuncDef: ModuleDefAST->FuncDefList)
    {
        cout << "FuncName:" << FuncDef->FuncName << endl;
    }
    llvm::LLVMContext Context;
    CodeGenContext CodeGenContext = ModuleDefAST->StartGenCode(Context);
    //    for (const auto& Token: Result)
    //    {
    //        cout << "TokenType:" << int(Token.TokenType)<< "TokenStr:" << Token.TokenBuffer << endl;
    //    }
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    CodeGenContext.Module->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        check(false);
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    //todo 这里编译为static链接会报错
    auto RM =llvm::Reloc::Model::PIC_;
    auto TheTargetMachine =
        Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    CodeGenContext.Module->setDataLayout(TheTargetMachine->createDataLayout());

    auto Filename = R"(G:\github\Seres\src\cmake-build-debug\main.o)";
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CodeGenFileType::ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*CodeGenContext.Module);
    dest.flush();

    llvm::outs() << "Wrote " << Filename << "\n";
    return 0;
}