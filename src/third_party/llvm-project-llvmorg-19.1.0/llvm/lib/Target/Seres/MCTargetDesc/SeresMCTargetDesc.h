//===-- SeresMCTargetDesc.h - Seres Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Seres specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Seres_MCTARGETDESC_SeresMCTARGETDESC_H
#define LLVM_LIB_TARGET_Seres_MCTARGETDESC_SeresMCTARGETDESC_H

#include "SeresConfig.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class Target;
class Triple;

extern Target TheSeresTarget;
extern Target TheSereselTarget;

} // End llvm namespace

// Defines symbolic names for Seres registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "SeresGenRegisterInfo.inc"

// Defines symbolic names for the Seres instructions.
#define GET_INSTRINFO_ENUM
#include "SeresGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "SeresGenSubtargetInfo.inc"

#endif

