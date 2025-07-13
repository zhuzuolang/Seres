//===-- Seres.td - Describe the Seres Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Without this will have error: 'Seres32I' is not a recognized processor for
//  this target (ignoring processor)
//===----------------------------------------------------------------------===//
// Seres Subtarget features                                                    //
//===----------------------------------------------------------------------===//

def FeatureChapter3_1  : SubtargetFeature<"ch3_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_2  : SubtargetFeature<"ch3_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_3  : SubtargetFeature<"ch3_3", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_4  : SubtargetFeature<"ch3_4", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_5  : SubtargetFeature<"ch3_5", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter4_1  : SubtargetFeature<"ch4_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter4_2  : SubtargetFeature<"ch4_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter5_1  : SubtargetFeature<"ch5_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter6_1  : SubtargetFeature<"ch6_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter7_1  : SubtargetFeature<"ch7_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter8_1  : SubtargetFeature<"ch8_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter8_2  : SubtargetFeature<"ch8_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter9_1  : SubtargetFeature<"ch9_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter9_2  : SubtargetFeature<"ch9_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter9_3  : SubtargetFeature<"ch9_3", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter10_1 : SubtargetFeature<"ch10_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter11_1 : SubtargetFeature<"ch11_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter11_2 : SubtargetFeature<"ch11_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter12_1 : SubtargetFeature<"ch12_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapterAll  : SubtargetFeature<"chall", "HasChapterDummy", "true",
                                "Enable Chapter instructions.",
                                [FeatureChapter3_1, FeatureChapter3_2, 
                                 FeatureChapter3_3, FeatureChapter3_4, 
                                 FeatureChapter3_5, 
                                 FeatureChapter4_1, FeatureChapter4_2, 
                                 FeatureChapter5_1, FeatureChapter6_1, 
                                 FeatureChapter7_1, FeatureChapter8_1, 
                                 FeatureChapter8_2, FeatureChapter9_1, 
                                 FeatureChapter9_2, FeatureChapter9_3, 
                                 FeatureChapter10_1, 
                                 FeatureChapter11_1, FeatureChapter11_2, 
                                 FeatureChapter12_1]>;

def FeatureCmp         : SubtargetFeature<"cmp", "HasCmp", "true",
                                "Enable 'cmp' instructions.">;
def FeatureSlt         : SubtargetFeature<"slt", "HasSlt", "true",
                                "Enable 'slt' instructions.">;
def FeatureSeres32I     : SubtargetFeature<"Seres32I", "SeresArchVersion",
                                "Seres32I", "Seres32I ISA Support",
                                [FeatureCmp, FeatureChapterAll]>;
def FeatureSeres32II    : SubtargetFeature<"Seres32II", "SeresArchVersion",
                               "Seres32II", "Seres32II ISA Support (slt)",
                                [FeatureCmp, FeatureSlt, FeatureChapterAll]>;

//===----------------------------------------------------------------------===//
// Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "SeresSchedule.td"
include "SeresInstrInfo.td"

def SeresInstrInfo : InstrInfo;
//===----------------------------------------------------------------------===//
// Seres processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, SeresGenericItineraries, Features>;

def : Proc<"Seres32I",  [FeatureSeres32I]>;
def : Proc<"Seres32II", [FeatureSeres32II]>;
// Above make SeresGenSubtargetInfo.inc set feature bit as the following order
// enum {
//   FeatureCmp =  1ULL << 0,
//   FeatureSeres32I =  1ULL << 1,
//   FeatureSeres32II =  1ULL << 2,
//   FeatureSlt =  1ULL << 3
// };

// Will generate SeresGenAsmWrite.inc included by SeresInstPrinter.cpp, contents
//  as follows,
// void SeresInstPrinter::printInstruction(const MCInst *MI, raw_ostream &O) {...}
// const char *SeresInstPrinter::getRegisterName(unsigned RegNo) {...}
def Seres : Target {
// def SeresInstrInfo : InstrInfo as before.
  let InstructionSet = SeresInstrInfo;
}

