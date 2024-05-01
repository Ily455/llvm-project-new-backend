//===-- RISCPIMCAsmInfo.h - RISCPI Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the RISCPIMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_MCTARGETDESC_RISCPIMCASMINFO_H
#define LLVM_LIB_TARGET_RISCPI_MCTARGETDESC_RISCPIMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

class RISCPIMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit RISCPIMCAsmInfo(const Triple &TheTriple);
};

} // namespace llvm

#endif // end LLVM_LIB_TARGET_RISCPI_MCTARGETDESC_RISCPIMCASMINFO_H
