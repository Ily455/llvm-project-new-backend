//===---- RISCPIISelDAGToDAG.h - A Dag to Dag Inst Selector for RISCPI ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the RISCPI target.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_RISCPIISELDAGTODAG_H
#define LLVM_LIB_TARGET_RISCPI_RISCPIISELDAGTODAG_H

#include "RISCPISubtarget.h"
#include "RISCPITargetMachine.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
class RISCPIDAGToDAGISel : public SelectionDAGISel {
public:
  explicit RISCPIDAGToDAGISel(RISCPITargetMachine &TM, CodeGenOpt::Level OL)
      : SelectionDAGISel(TM, OL), Subtarget(nullptr) {}

  // Pass Name
  StringRef getPassName() const override {
    return "CPU0 DAG->DAG Pattern Instruction Selection";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void Select(SDNode *Node) override;

#include "RISCPIGenDAGISel.inc"

private:
  const RISCPISubtarget *Subtarget;
};
}

#endif // end LLVM_LIB_TARGET_RISCPI_RISCPIISELDAGTODAG_H
