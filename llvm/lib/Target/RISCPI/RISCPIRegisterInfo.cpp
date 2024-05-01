//===-- RISCPIRegisterInfo.cpp - RISCPI Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RISCPI implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "RISCPIRegisterInfo.h"
#include "RISCPISubtarget.h"
#include "llvm/Support/Debug.h"

#define GET_REGINFO_TARGET_DESC
#include "RISCPIGenRegisterInfo.inc"

#define DEBUG_TYPE "riscpi-reginfo"

using namespace llvm;

RISCPIRegisterInfo::RISCPIRegisterInfo(const RISCPISubtarget &ST)
  : RISCPIGenRegisterInfo(RISCPI::X1, /*DwarfFlavour*/0, /*EHFlavor*/0,
                         /*PC*/0), Subtarget(ST) {}

const MCPhysReg *
RISCPIRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return RISCPI_CalleeSavedRegs_SaveList;
}

const TargetRegisterClass *RISCPIRegisterInfo::intRegClass(unsigned Size) const {
  return &RISCPI::GPRRegClass;
}

const uint32_t *
RISCPIRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID) const {
  return RISCPI_CalleeSavedRegs_RegMask;
}

BitVector RISCPIRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  markSuperRegs(Reserved, RISCPI::X0); // zero
  markSuperRegs(Reserved, RISCPI::X2); // sp
  markSuperRegs(Reserved, RISCPI::X3); // gp
  markSuperRegs(Reserved, RISCPI::X4); // tp

  return Reserved;
}

void RISCPIRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                           int SPAdj,
                                           unsigned FIOperandNum,
                                           RegScavenger *RS) const {
  llvm_unreachable("Unsupported eliminateFrameIndex");
}

bool
RISCPIRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
RISCPIRegisterInfo::requiresFrameIndexScavenging(
                                            const MachineFunction &MF) const {
  return true;
}

bool
RISCPIRegisterInfo::requiresFrameIndexReplacementScavenging(
                                            const MachineFunction &MF) const {
  return true;
}

bool
RISCPIRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

Register RISCPIRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  llvm_unreachable("Unsupported getFrameRegister");
}

