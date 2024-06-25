#include "CommonSubexpressionEliminationPass.hpp"

#include<map>
using namespace llvm;

PreservedAnalyses
CommonSubexpressionEliminationPass::run(Module& mod, ModuleAnalysisManager& mam)
{
    int cseCount = 0;

    // 遍历所有函数
    for (auto& func : mod) {
        // 遍历每个函数的基本块
        for (auto& bb : func) {
            std::map<std::pair<Value*, Value*>, Value*> binOpToResult;
            // 遍历每个基本块的指令
            for (auto& inst : bb) {
                // 判断当前指令是否是二元运算指令
                if (auto binOp = dyn_cast<BinaryOperator>(&inst)) {
                    // 获取二元运算指令的左右操作数
                    Value* lhs = binOp->getOperand(0);
                    Value* rhs = binOp->getOperand(1);
                    // 判断左右操作数是否已经计算过
                    auto it = binOpToResult.find({lhs, rhs});
                    if (it != binOpToResult.end()) {
                        // 若已经计算过，则直接替换为之前的结果
                        binOp->replaceAllUsesWith(it->second);
                        ++cseCount;
                    } else {
                        // 若未计算过，则将结果存入map
                        binOpToResult[{lhs, rhs}] = &inst;
                    }
                }
            }
        }
    }
    mOut << "CommonSubexpressionElimination: " << cseCount << " instructions removed\n";
    return PreservedAnalyses::all();
}