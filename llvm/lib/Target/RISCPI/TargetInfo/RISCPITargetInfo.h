//===---- RISCPITargetInfo.h - RISCPI Target Implementation -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCPI_TARGETINFO_RISCPITARGETINFO_H
#define LLVM_LIB_TARGET_RISCPI_TARGETINFO_RISCPITARGETINFO_H

namespace llvm {

class Target;

Target &getTheRISCPITarget();

} // namespace llvm

#endif // LLVM_LIB_TARGET_RISCPI_TARGETINFO_RISCPITARGETINFO_H
