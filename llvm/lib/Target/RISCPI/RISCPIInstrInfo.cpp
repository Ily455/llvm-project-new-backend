//===-- RISCPIInstrInfo.cpp - RISCPI Instruction Information ----------------===//
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

#include "RISCPIInstrInfo.h"

#include "RISCPITargetMachine.h"
#include "RISCPIMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "riscpi-instrinfo"

#define GET_INSTRINFO_CTOR_DTOR
#include "RISCPIGenInstrInfo.inc"

RISCPIInstrInfo::RISCPIInstrInfo(const RISCPISubtarget &STI)
    : RISCPIGenInstrInfo(RISCPI::ADJCALLSTACKDOWN, RISCPI::ADJCALLSTACKUP),
      Subtarget(STI)
{
}
