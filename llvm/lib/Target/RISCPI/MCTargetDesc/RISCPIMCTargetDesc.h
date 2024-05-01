//===-- RISCPIMCTargetDesc.h - RISCPI Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides RISCPI specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_MCTARGETDESC_RISCPIMCTARGETDESC_H
#define LLVM_LIB_TARGET_RISCPI_MCTARGETDESC_RISCPIMCTARGETDESC_H

#include "RISCPIBaseInfo.h"

// Defines symbolic names for RISCPI registers. This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "RISCPIGenRegisterInfo.inc"

// Defines symbolic names for the RISCPI instructions.
#define GET_INSTRINFO_ENUM
#include "RISCPIGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "RISCPIGenSubtargetInfo.inc"

#endif // end LLVM_LIB_TARGET_RISCPI_MCTARGETDESC_RISCPIMCTARGETDESC_H
