//===-- SeresMCTargetDesc.cpp - Seres Target Descriptions -------------------===//
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

#include "SeresMCTargetDesc.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "SeresGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "SeresGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "SeresGenRegisterInfo.inc"

//@2 {
extern "C" void LLVMInitializeSeresTargetMC() {

}
//@2 }

