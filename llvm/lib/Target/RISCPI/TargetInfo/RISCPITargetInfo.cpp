//===-- RISCPITargetInfo.cpp - RISCPI Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/RISCPITargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheRISCPITarget() {
  static Target TheRISCPITarget;
  return TheRISCPITarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRISCPITargetInfo() {
  RegisterTarget<Triple::riscpi> X(getTheRISCPITarget(), "riscpi",
                                  "32-bit RISC-V", "RISCPI");
}
