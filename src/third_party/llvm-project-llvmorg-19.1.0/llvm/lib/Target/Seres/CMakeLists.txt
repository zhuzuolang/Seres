add_llvm_component_group(Seres)

set(LLVM_TARGET_DEFINITIONS SeresOther.td)

# Generate SeresGenRegisterInfo.inc and SeresGenInstrInfo.inc which included by
#  your hand code C++ files. 
# SeresGenRegisterInfo.inc came from SeresRegisterInfo.td, SeresGenInstrInfo.inc
#  came from SeresInstrInfo.td.
tablegen(LLVM SeresGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM SeresGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM SeresGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM SeresGenMCPseudoLowering.inc -gen-pseudo-lowering)

# SeresCommonTableGen must be defined
add_public_tablegen_target(SeresCommonTableGen)

# SeresCodeGen should match with LLVMBuild.txt SeresCodeGen
add_llvm_target(SeresCodeGen
  SeresTargetMachine.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  Core
  MC
  SeresDesc
  SeresInfo
  SelectionDAG
  Support
  Target
  GlobalISel

  ADD_TO_COMPONENT
  Seres
  )

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)

