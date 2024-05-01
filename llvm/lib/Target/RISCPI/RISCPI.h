//=== RISCPI.h - Top-level interface for RISCPI representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM RISCPI backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_RISCPI_H
#define LLVM_LIB_TARGET_RISCPI_RISCPI_H

#include "MCTargetDesc/RISCPIMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class FunctionPass;

  // Declare functions to create passes here!

} // end namespace llvm;

#endif // end LLVM_LIB_TARGET_RISCPI_RISCPI_H
