//===-- SeresTargetInfo.cpp - Seres Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Seres.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target llvm::TheSeresTarget, llvm::TheSereselTarget;

extern "C" void LLVMInitializeSeresTargetInfo() {
  RegisterTarget<Triple::Seres,
        /*HasJIT=*/true> X(TheSeresTarget, "Seres", "Seres (32-bit big endian)", "Seres");

  RegisterTarget<Triple::Seresel,
        /*HasJIT=*/true> Y(TheSereselTarget, "Seresel", "Seres (32-bit little endian)", "Seres");
}

