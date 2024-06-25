#include "AlgebraicIdentities.hpp"

using namespace llvm;

PreservedAnalyses
AlgebraicIdentities::run(Module& mod, ModuleAnalysisManager& mam)
{
  int algebraicIdentitiesCount = 0;

  for (auto& func : mod) {
    for (auto& bb : func) {
      for (auto& inst : bb) {
        if (auto binOp = dyn_cast<BinaryOperator>(&inst)) {
          Value* lhs = binOp->getOperand(0);
          Value* rhs = binOp->getOperand(1);
          switch (binOp->getOpcode()) {
            case Instruction::Add: {
              if (auto constRhs = dyn_cast<ConstantInt>(rhs)) {
                if (constRhs->isZero()) {
                  binOp->replaceAllUsesWith(lhs);
                  ++algebraicIdentitiesCount;
                }
              }
              break;
            }
            case Instruction::Sub: {
              if (auto constRhs = dyn_cast<ConstantInt>(rhs)) {
                if (constRhs->isZero()) {
                  binOp->replaceAllUsesWith(lhs);
                  ++algebraicIdentitiesCount;
                }
              }
              break;
            }
            case Instruction::Mul: {
              if (auto constRhs = dyn_cast<ConstantInt>(rhs)) {
                if (constRhs->isOne()) {
                  binOp->replaceAllUsesWith(lhs);
                  ++algebraicIdentitiesCount;
                }
              }
              break;
            }
            case Instruction::UDiv:
            case Instruction::SDiv: {
              if (auto constRhs = dyn_cast<ConstantInt>(rhs)) {
                if (constRhs->isOne()) {
                  binOp->replaceAllUsesWith(lhs);
                  ++algebraicIdentitiesCount;
                }
              }
              break;
            }
            default:
              break;
          }
        }
      }
    }
  }

  mOut << "AlgebraicIdentities: " << algebraicIdentitiesCount << " identities applied\n";
  return PreservedAnalyses::all();
}