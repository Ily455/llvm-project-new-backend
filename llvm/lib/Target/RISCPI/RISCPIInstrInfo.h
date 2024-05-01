//===-- RISCPIInstrInfo.h - RISCPI Instruction Information ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RISCPI implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_RISCPIINSTRINFO_H
#define LLVM_LIB_TARGET_RISCPI_RISCPIINSTRINFO_H

#include "RISCPI.h"
#include "RISCPIRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "RISCPIGenInstrInfo.inc"

namespace llvm {

class RISCPIInstrInfo : public RISCPIGenInstrInfo {
public:
  explicit RISCPIInstrInfo(const RISCPISubtarget &STI);

protected:
  const RISCPISubtarget &Subtarget;
};
}

#endif // end LLVM_LIB_TARGET_RISCPI_RISCPIINSTRINFO_H
