set(LLVM_TARGET_DEFINITIONS RISCPI.td)

tablegen(LLVM RISCPIGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM RISCPIGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM RISCPIGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM RISCPIGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM RISCPIGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM RISCPIGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM RISCPIGenCallingConv.inc -gen-callingconv)
tablegen(LLVM RISCPIGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM RISCPIGenMCPseudoLowering.inc -gen-pseudo-lowering)

# RISCPICommonTableGen must be defined
add_public_tablegen_target(RISCPICommonTableGen)

# RISCPICodeGen should match with LLVMBuild.txt RISCPICodeGen
add_llvm_target(RISCPICodeGen
  RISCPIAsmPrinter.cpp
  RISCPIFrameLowering.cpp
  RISCPIInstrInfo.cpp
  RISCPIISelLowering.cpp
  RISCPIRegisterInfo.cpp
  RISCPISubtarget.cpp
  RISCPITargetObjectFile.cpp
  RISCPIISelDAGToDAG.cpp
  RISCPITargetMachine.cpp
)

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)