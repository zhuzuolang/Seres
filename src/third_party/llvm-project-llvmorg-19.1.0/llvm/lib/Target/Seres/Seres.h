//===-- Seres.h - Top-level interface for Seres representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Seres back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Seres_Seres_H
#define LLVM_LIB_TARGET_Seres_Seres_H

#include "SeresConfig.h"
#include "MCTargetDesc/SeresMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class SeresTargetMachine;
  class FunctionPass;

} // end namespace llvm;

#endif

