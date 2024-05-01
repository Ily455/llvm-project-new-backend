//=== RISCPI.td - Describe the RISCPI Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top tablegen file that pulls in code from the LLVM framework along with
// code specific to this target.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Calling Conv, Instruction Descriptions, Register information, etc
//===----------------------------------------------------------------------===//

include "RISCPISchedule.td"
include "RISCPIRegisterInfo.td"
include "RISCPICallingConv.td"
include "RISCPIInstrInfo.td"

def RISCPIInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// RISCPI processors supported
//===----------------------------------------------------------------------===//

def : ProcessorModel<"generic",  NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// RISCPI Target declaration
//===----------------------------------------------------------------------===//

def RISCPI : Target {
  let InstructionSet = RISCPIInstrInfo;
}
