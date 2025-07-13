//===-- SeresRegisterInfo.td - Seres Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Seres register file
//===----------------------------------------------------------------------===//

// We have banks of 16 registers each.
class SeresReg<bits<16> Enc, string n> : Register<n> {
  // For tablegen(... -gen-emitter)  in CMakeLists.txt
  let HWEncoding = Enc;
  
  let Namespace = "Seres";
}

// Seres CPU Registers
class SeresGPRReg<bits<16> Enc, string n> : SeresReg<Enc, n>;

// Co-processor 0 Registers
class SeresC0Reg<bits<16> Enc, string n> : SeresReg<Enc, n>;

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "Seres" in {
  //@ General Purpose Registers
  def ZERO : SeresGPRReg<0,  "zero">, DwarfRegNum<[0]>;
  def AT   : SeresGPRReg<1,  "1">,    DwarfRegNum<[1]>;
  def V0   : SeresGPRReg<2,  "2">,    DwarfRegNum<[2]>;
  def V1   : SeresGPRReg<3,  "3">,    DwarfRegNum<[3]>;
  def A0   : SeresGPRReg<4,  "4">,    DwarfRegNum<[4]>;
  def A1   : SeresGPRReg<5,  "5">,    DwarfRegNum<[5]>;
  def T9   : SeresGPRReg<6,  "t9">,   DwarfRegNum<[6]>;
  def T0   : SeresGPRReg<7,  "7">,    DwarfRegNum<[7]>;
  def T1   : SeresGPRReg<8,  "8">,    DwarfRegNum<[8]>;
  def S0   : SeresGPRReg<9,  "9">,    DwarfRegNum<[9]>;
  def S1   : SeresGPRReg<10, "10">,   DwarfRegNum<[10]>;
  def GP   : SeresGPRReg<11, "gp">,   DwarfRegNum<[11]>;
  def FP   : SeresGPRReg<12, "fp">,   DwarfRegNum<[12]>;
  def SP   : SeresGPRReg<13, "sp">,   DwarfRegNum<[13]>;
  def LR   : SeresGPRReg<14, "lr">,   DwarfRegNum<[14]>;
  def SW   : SeresGPRReg<15, "sw">,   DwarfRegNum<[15]>;
//  def MAR  : Register< 16, "mar">,  DwarfRegNum<[16]>;
//  def MDR  : Register< 17, "mdr">,  DwarfRegNum<[17]>;

  def PC   : SeresC0Reg<0, "pc">,  DwarfRegNum<[20]>;
  def EPC  : SeresC0Reg<1, "epc">, DwarfRegNum<[21]>;
}

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"Seres", [i32], 32, (add
  // Reserved
  ZERO, AT, 
  // Return Values and Arguments
  V0, V1, A0, A1, 
  // Not preserved across procedure calls
  T9, T0, T1,
  // Callee save
  S0, S1,
  // Reserved
  GP, FP, 
  SP, LR, SW)>;

//@Status Registers class
def SR     : RegisterClass<"Seres", [i32], 32, (add SW)>;

//@Co-processor 0 Registers class
def C0Regs : RegisterClass<"Seres", [i32], 32, (add PC, EPC)>;

