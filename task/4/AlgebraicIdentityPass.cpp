#include "AlgebraicIdentityPass.hpp"
#include <set>

using namespace llvm;

PreservedAnalyses AlgebraicIdentityPass::run(Module &mod, ModuleAnalysisManager &mam) {
    int count = 0;

    std::set<Instruction *> instToErase;
    // 遍历所有函数
    for (auto &func : mod) {
        // 遍历每个函数的基本块
        for (auto &bb : func) {
            // 遍历每个基本块的指令
            for (auto &inst : bb) {
                // 判断当前指令是否是二元运算指令
                if (auto binOp = dyn_cast<BinaryOperator>(&inst)) {
                    // 获取二元运算指令的左右操作数，并尝试转换为常整数
                    Value *lhs = binOp->getOperand(0);
                    Value *rhs = binOp->getOperand(1);
                    auto constLhs = dyn_cast<ConstantInt>(lhs);
                    auto constRhs = dyn_cast<ConstantInt>(rhs);

                    auto op = binOp->getOpcode();
                    
                    llvm::Value *result = nullptr;
                    if (constRhs) {

                        switch (op) {
                            case Instruction::Add:
                                if (constRhs->isZero())
                                    result = lhs;
                                break;
                            case Instruction::Sub:
                                if (constRhs->isZero())
                                    result = lhs;
                                break;
                            case Instruction::Mul:
                                if (constRhs->isZero())
                                    break;
                                else if (constRhs->equalsInt(1)) 
                                    result = lhs;
                                break;
                            case Instruction::SDiv:
                                if (constRhs->isZero() || constRhs->equalsInt(1))
                                    result = lhs;
                                break;
                            case Instruction::SRem:
                                if (constRhs->equalsInt(1))
                                    result = ConstantInt::get(binOp->getType(), 0); 
                                break;
                            case Instruction::Shl:
                                if (constRhs->isZero())
                                    result = lhs;
                                break;
                            case Instruction::LShr:
                                if (constRhs->isZero())
                                    result = lhs;
                                break;
                            case Instruction::AShr:
                                if (constRhs->isZero())
                                    result = lhs;
                                break;
                            case Instruction::And:
                                if (constRhs->isZero())
                                    result = ConstantInt::get(binOp->getType(), 0);
                                break;
                            case Instruction::Or:
                                if (constRhs->isZero())
                                    result = lhs;
                                break;
                            default:
                                break;
                        }

                        if (result) {
                            // 替换指令
                            binOp->replaceAllUsesWith(result);
                            instToErase.insert(binOp);
                            ++count;
                        }

                    }

                    if (!result&&constLhs) {

                        switch (op) {
                            case Instruction::Add:
                                if (constLhs->isZero())
                                    result = rhs;
                                break;
                            case Instruction::Mul:
                                if (constLhs->isZero())
                                    result = ConstantInt::get(binOp->getType(), 0);
                                else if (constLhs->equalsInt(1)) 
                                    result = rhs;
                                break;
                            case Instruction::SDiv:
                                if (constLhs->isZero() )
                                    result = lhs;
                                break;
                            case Instruction::Shl:
                                if (constLhs->isZero())
                                    result = ConstantInt::get(binOp->getType(), 0);
                                break;
                            case Instruction::LShr:
                                if (constLhs->isZero())
                                    result = ConstantInt::get(binOp->getType(), 0);
                                break;
                            case Instruction::AShr:
                                if (constLhs->isZero())
                                    result = ConstantInt::get(binOp->getType(), 0);
                                break;
                            case Instruction::And:
                                if (constLhs->isZero())
                                    result = ConstantInt::get(binOp->getType(), 0);
                                break;
                            case Instruction::Or:
                                if (constLhs->isZero())
                                    result = rhs;
                                break;
                            default:
                                break;
                        }

                        if (result) {
                            // 替换指令
                            binOp->replaceAllUsesWith(result);
                            instToErase.insert(binOp);
                            ++count;
                        }

                    }
                }
            }


        }
    }

    // 删除指令
    for (Instruction *inst : instToErase) {
        inst->eraseFromParent();
    }
    return PreservedAnalyses::all();
}