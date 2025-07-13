# MCTargetDesc/CMakeLists.txt
add_llvm_component_library(LLVMSeresDesc
  SeresMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  SeresInfo
  Support

  ADD_TO_COMPONENT
  Seres
  )

