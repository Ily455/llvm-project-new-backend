//===-- RISCPIISelLowering.cpp - RISCPI DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that RISCPI uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "RISCPIISelLowering.h"
#include "RISCPISubtarget.h"
#include "RISCPITargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/Debug.h"
#include <cassert>

using namespace llvm;

#define DEBUG_TYPE "riscpi-isellower"

#include "RISCPIGenCallingConv.inc"

RISCPITargetLowering::RISCPITargetLowering(const TargetMachine &TM,
                                         const RISCPISubtarget &STI)
    : TargetLowering(TM), Subtarget(STI)
{
  // Set up the register classes
  addRegisterClass(MVT::i32, &RISCPI::GPRRegClass);

  // Must, computeRegisterProperties - Once all of the register classes are
  // added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());

  // Set scheduling preference
  setSchedulingPreference(Sched::RegPressure);

  setStackPointerRegisterToSaveRestore(RISCPI::X2);

  // Use i32 for setcc operations results (slt, sgt, ...).
  setBooleanContents(ZeroOrOneBooleanContent);

  // Arithmetic operations
  setOperationAction(ISD::SDIVREM,   MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM,   MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);

  setOperationAction(ISD::SHL_PARTS, MVT::i32, Custom);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Custom);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Custom);

  setOperationAction(ISD::ROTL,  MVT::i32, Expand);
  setOperationAction(ISD::ROTR,  MVT::i32, Expand);
  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ,  MVT::i32, Expand);
  setOperationAction(ISD::CTLZ,  MVT::i32, Expand);
  setOperationAction(ISD::CTPOP, MVT::i32, Expand);

  // Address resolution and constant pool
  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
  setOperationAction(ISD::BlockAddress,  MVT::i32, Custom);
  setOperationAction(ISD::ConstantPool,  MVT::i32, Custom);

  // Set minimum and preferred function alignment (log2)
  setMinFunctionAlignment(Align(1));
  setPrefFunctionAlignment(Align(1));

  // Set preferred loop alignment (log2)
  setPrefLoopAlignment(Align(1));
}

const char *RISCPITargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case RISCPIISD::Ret: return "RISCPIISD::Ret";
  default:            return NULL;
  }
}

void RISCPITargetLowering::ReplaceNodeResults(SDNode *N,
                                             SmallVectorImpl<SDValue> &Results,
                                             SelectionDAG &DAG) const {
  switch (N->getOpcode()) {
  default:
    llvm_unreachable("Don't know how to custom expand this!");
  }
}

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

// The BeyondRISC calling convention parameter registers.
static const MCPhysReg GPRArgRegs[] = {
  RISCPI::X0, RISCPI::X1, RISCPI::X2, RISCPI::X3
};

/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue RISCPITargetLowering::LowerFormalArguments(
                                    SDValue Chain,
                                    CallingConv::ID CallConv,
                                    bool isVarArg,
                                    const SmallVectorImpl<ISD::InputArg> &Ins,
                                    const SDLoc &dl, SelectionDAG &DAG,
                                    SmallVectorImpl<SDValue> &InVals) const {
  assert((CallingConv::C == CallConv || CallingConv::Fast == CallConv) &&
		 "Unsupported CallingConv to FORMAL_ARGS");

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, RISCPI_CCallingConv);

  SmallVector<SDValue, 16> ArgValues;
  SDValue ArgValue;
  Function::const_arg_iterator CurOrigArg = MF.getFunction().arg_begin();
  unsigned CurArgIdx = 0;

  // Calculate the amount of stack space that we need to allocate to store
  // byval and variadic arguments that are passed in registers.
  // We need to know this before we allocate the first byval or variadic
  // argument, as they will be allocated a stack slot below the CFA (Canonical
  // Frame Address, the stack pointer at entry to the function).
  unsigned ArgRegBegin = RISCPI::X4;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    if (CCInfo.getInRegsParamsProcessed() >= CCInfo.getInRegsParamsCount())
      break;

    CCValAssign &VA = ArgLocs[i];
    unsigned Index = VA.getValNo();
    ISD::ArgFlagsTy Flags = Ins[Index].Flags;
    if (!Flags.isByVal())
      continue;

    assert(VA.isMemLoc() && "unexpected byval pointer in reg");
    unsigned RBegin, REnd;
    CCInfo.getInRegsParamInfo(CCInfo.getInRegsParamsProcessed(), RBegin, REnd);
    ArgRegBegin = std::min(ArgRegBegin, RBegin);

    CCInfo.nextInRegsParam();
  }
  CCInfo.rewindByValRegsInfo();

  int lastInsIndex = -1;
  if (isVarArg && MFI.hasVAStart()) {
    unsigned RegIdx = CCInfo.getFirstUnallocated(GPRArgRegs);
    if (RegIdx != array_lengthof(GPRArgRegs))
      ArgRegBegin = std::min(ArgRegBegin, (unsigned)GPRArgRegs[RegIdx]);
  }

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (Ins[VA.getValNo()].isOrigArg()) {
      std::advance(CurOrigArg,
                   Ins[VA.getValNo()].getOrigArgIndex() - CurArgIdx);
      CurArgIdx = Ins[VA.getValNo()].getOrigArgIndex();
    }
    // Arguments stored in registers.
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();

      if (VA.needsCustom()) {
        llvm_unreachable("Custom val assignment not supported by "
                         "FORMAL_ARGUMENTS Lowering");
      } else {
        const TargetRegisterClass *RC;

        if (RegVT == MVT::i32)
          RC = &RISCPI::GPRRegClass;
        else
          llvm_unreachable("RegVT not supported by FORMAL_ARGUMENTS Lowering");

        // Transform the arguments in physical registers into virtual ones.
        unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
        ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);
      }

      // If this is an 8 or 16-bit value, it is really passed promoted
      // to 32 bits.  Insert an assert[sz]ext to capture this, then
      // truncate to the right size.
      switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::BCvt:
        ArgValue = DAG.getNode(ISD::BITCAST, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::SExt:
        ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::ZExt:
        ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      }

      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()
      // sanity check
      assert(VA.isMemLoc());
      assert(VA.getValVT() != MVT::i64 && "i64 should already be lowered");

      int index = VA.getValNo();

      // Some Ins[] entries become multiple ArgLoc[] entries.
      // Process them only once.
      if (index != lastInsIndex)
      {
        llvm_unreachable("Cannot retrieve arguments from the stack");
      }
    }
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool RISCPITargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                                MachineFunction &MF, bool isVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RISCPI_CRetConv);
}

SDValue RISCPITargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
  llvm_unreachable("Cannot lower call");
}

SDValue
RISCPITargetLowering::LowerReturn(SDValue Chain,
                                 CallingConv::ID CallConv, bool isVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &dl, SelectionDAG &DAG) const {
  // CCValAssign - represent the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slots.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analyze outgoing return values.
  CCInfo.AnalyzeReturn(Outs, RISCPI_CRetConv);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps;
  RetOps.push_back(Chain); // Operand #0 = Chain (updated below)

  // Copy the result values into the output registers.
  for (unsigned i = 0, realRVLocIdx = 0;
       i != RVLocs.size();
       ++i, ++realRVLocIdx) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    SDValue Arg = OutVals[realRVLocIdx];
    bool ReturnF16 = false;

    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: break;
    case CCValAssign::BCvt:
      if (!ReturnF16)
        Arg = DAG.getNode(ISD::BITCAST, dl, VA.getLocVT(), Arg);
      break;
    }

    if (VA.needsCustom()) {
      llvm_unreachable("Custom val assignment not supported by "
                       "RETURN Lowering");
    } else {
      Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), Arg, Flag);
    }

    // Guarantee that all emitted copies are stuck together, avoiding something
    // bad.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(),
                                     ReturnF16 ? MVT::f16 : VA.getLocVT()));
  }

  // Update chain and glue.
  RetOps[0] = Chain;
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(RISCPIISD::Ret, dl, MVT::Other, RetOps);
}

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

SDValue
RISCPITargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {
  llvm_unreachable("Unsupported global address");
}

SDValue
RISCPITargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const {
  llvm_unreachable("Unsupported block address");
}

SDValue
RISCPITargetLowering::LowerConstantPool(SDValue Op, SelectionDAG &DAG) const {
  llvm_unreachable("Unsupported constant pool");
}

SDValue
RISCPITargetLowering::LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const {
  return SDValue();
}

SDValue
RISCPITargetLowering::LowerShlParts(SDValue Op, SelectionDAG &DAG) const {
  assert(Op.getNumOperands() == 3 && "Not a long shift");

  EVT VT = Op.getValueType();
  unsigned VTBits = VT.getSizeInBits();
  SDLoc DL(Op);

  // if Shamt-32 < 0: // Shamt < 32
  //   Lo = Lo << Shamt
  //   Hi = (Hi << Shamt) | ((Lo >>u 1) >>u (32-1 - Shamt))
  // else:
  //   Lo = 0
  //   Hi = Lo << (Shamt-32)

  SDValue Lo = Op.getOperand(0);
  SDValue Hi = Op.getOperand(1);
  SDValue Shamt = Op.getOperand(2);

  // Precompute a node with ...
  // ... constant value 0
  SDValue Zero = DAG.getConstant(0, DL, VT);
  // ... constant value 1
  SDValue One = DAG.getConstant(1, DL, VT);
  // ... -32
  SDValue NegWsize = DAG.getConstant(-VTBits, DL, VT);
  // ... 32 - 1
  SDValue WsizeMinus1 = DAG.getConstant(VTBits - 1, DL, VT);
  // ... shamt - 32
  SDValue ShamtMinusWsize = DAG.getNode(ISD::ADD, DL, VT, Shamt, NegWsize);
  // ... (32 - 1) - shamt
  SDValue WsizeMinus1MinusShamt =
      DAG.getNode(ISD::SUB, DL, VT, WsizeMinus1, Shamt);

  // Compute the 'then' part of the if
  // Lo << Shamt
  SDValue LoTrue = DAG.getNode(ISD::SHL, DL, VT, Lo, Shamt);
  // Lo >> 1
  SDValue LoShr1 = DAG.getNode(ISD::SRL, DL, VT, Lo, One);
  // (Lo >> 1) >> (32-1 - Shamt)
  SDValue HiLsb = DAG.getNode(ISD::SRL, DL, VT, LoShr1, WsizeMinus1MinusShamt);
  // Hi << Shamt
  SDValue HiMsb = DAG.getNode(ISD::SHL, DL, VT, Hi, Shamt);
  // (Hi << Shamt) | ((Lo >>u 1) >>u (32-1 - Shamt))
  SDValue HiTrue = DAG.getNode(ISD::OR, DL, VT, HiMsb, HiLsb);

  // Compute the 'else' part of the if
  SDValue LoFalse = Zero;
  SDValue HiFalse = DAG.getNode(ISD::SHL, DL, VT, Lo, ShamtMinusWsize);

  // Compute the if condition
  SDValue CC = DAG.getSetCC(DL, VT, ShamtMinusWsize, Zero, ISD::SETLT);

  // and (finally) select the results based on the condition!
  Lo = DAG.getNode(ISD::SELECT, DL, VT, CC, LoTrue, LoFalse);
  Hi = DAG.getNode(ISD::SELECT, DL, VT, CC, HiTrue, HiFalse);

  SDValue Ops[2] = {Lo, Hi};
  return DAG.getMergeValues(Ops, DL);
}

SDValue
RISCPITargetLowering::LowerShrParts(SDValue Op, SelectionDAG &DAG,
                                   bool arith) const {
  assert(Op.getNumOperands() == 3 && "Not a long shift");

  EVT VT = Op.getValueType();
  unsigned VTBits = VT.getSizeInBits();
  SDLoc DL(Op);

  // SRA expansion:
  //   if Shamt-32 < 0: // Shamt < 32
  //     Lo = (Lo >>u Shamt) | ((Hi << 1) << (32-1 - Shamt))
  //     Hi = Hi >>s Shamt
  //   else:
  //     Lo = Hi >>s (Shamt-32);
  //     Hi = Hi >>s (32-1)
  //
  // SRL expansion:
  //   if Shamt-32 < 0: // Shamt < 32
  //     Lo = (Lo >>u Shamt) | ((Hi << 1) << (32-1 - Shamt))
  //     Hi = Hi >>u Shamt
  //   else:
  //     Lo = Hi >>u (Shamt-32);
  //     Hi = 0;

  SDValue Lo = Op.getOperand(0);
  SDValue Hi = Op.getOperand(1);
  SDValue Shamt = Op.getOperand(2);

  // The only differences between the SRA and SRL expansions are the right
  // shift operations: arithmetic for SRA and logical for SRL; and Hi
  unsigned ShrOp = arith ? ISD::SRA : ISD::SRL;

  // Precompute a node with ...
  // ... constant value 0
  SDValue Zero = DAG.getConstant(0, DL, VT);
  // ... constant value 1
  SDValue One = DAG.getConstant(1, DL, VT);
  // ... -32
  SDValue NegWsize = DAG.getConstant(-VTBits, DL, VT);
  // ... 32 - 1
  SDValue WsizeMinus1 = DAG.getConstant(VTBits - 1, DL, VT);
  // ... shamt - 32
  SDValue ShamtMinusWsize = DAG.getNode(ISD::ADD, DL, VT, Shamt, NegWsize);
  // ... (32 - 1) - shamt
  SDValue WsizeMinus1MinusShamt =
      DAG.getNode(ISD::SUB, DL, VT, WsizeMinus1, Shamt);

  // Compute the 'then' part of the if
  // Hi << 1
  SDValue HiShr1 = DAG.getNode(ISD::SHL, DL, VT, Lo, One);
  // (Hi << 1) << (32-1 - Shamt)
  SDValue LoMsb = DAG.getNode(ISD::SRL, DL, VT, HiShr1, WsizeMinus1MinusShamt);
  // Lo >>u Shamt
  SDValue LoLsb = DAG.getNode(ISD::SRL, DL, VT, Lo, Shamt);
  // (Lo >>u Shamt) | ((Hi << 1) << (32-1 - Shamt))
  SDValue LoTrue = DAG.getNode(ISD::OR, DL, VT, LoMsb, LoLsb);
  // Hi >>s Shamt
  SDValue HiTrue = DAG.getNode(ShrOp, DL, VT, Hi, Shamt);

  // Compute the 'else' part of the if
  SDValue LoFalse = DAG.getNode(ShrOp, DL, VT, Hi, ShamtMinusWsize);
  SDValue HiFalse =
      arith ? DAG.getNode(ISD::SRA, DL, VT, Hi, WsizeMinus1) : Zero;

  // Compute the if condition
  SDValue CC = DAG.getSetCC(DL, VT, ShamtMinusWsize, Zero, ISD::SETLT);

  // and (finally) select the results based on the condition!
  Lo = DAG.getNode(ISD::SELECT, DL, VT, CC, LoTrue, LoFalse);
  Hi = DAG.getNode(ISD::SELECT, DL, VT, CC, HiTrue, HiFalse);

  SDValue Ops[2] = {Lo, Hi};
  return DAG.getMergeValues(Ops, DL);
}

SDValue
RISCPITargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:                        llvm_unreachable("unimplemented operand");
  case ISD::GlobalAddress:        return LowerGlobalAddress(Op, DAG);
  case ISD::BlockAddress:         return LowerBlockAddress(Op, DAG);
  case ISD::ConstantPool:         return LowerConstantPool(Op, DAG);
  case ISD::RETURNADDR:           return LowerRETURNADDR(Op, DAG);
  case ISD::SHL_PARTS:            return LowerShlParts(Op, DAG);
  case ISD::SRL_PARTS:            return LowerShrParts(Op, DAG, false);
  case ISD::SRA_PARTS:            return LowerShrParts(Op, DAG, true);
  }
}
