//=== RISCPIMachineFunctionInfo.h - Private data used for RISCPI ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the RISCPI specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_RISCPIMACHINEFUNCTION_H
#define LLVM_LIB_TARGET_RISCPI_RISCPIMACHINEFUNCTION_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// RISCPIFunctionInfo - This class is derived from MachineFunction private
/// RISCPI target-specific information for each MachineFunction.
class RISCPIFunctionInfo : public MachineFunctionInfo {
private:
  MachineFunction &MF;

public:
  RISCPIFunctionInfo(MachineFunction &MF) : MF(MF) {}
};

} // end of namespace llvm

#endif // end LLVM_LIB_TARGET_RISCPI_RISCPIMACHINEFUNCTION_H
