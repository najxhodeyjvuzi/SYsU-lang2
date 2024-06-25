
#include "DeadInstEliminationPass.hpp"
#include <iostream>

using namespace llvm;

PreservedAnalyses DeadInstEliminationPass::run(Module &mod, ModuleAnalysisManager &mam) {
    int deadInstCount = 0;

    // 您原本的代码逻辑
    for (auto &func : mod) {
        for (auto &bb : func) {
            std::vector<Instruction*> instToErase;
            for (auto &inst : bb) {
                // 不使用inst.use_empty()，而是使用inst.hasNUses(0)

                if (inst.getNumUses()==0) {
                    instToErase.push_back(&inst);
                    ++deadInstCount;
                }
            }
            for (auto inst : instToErase) {
                inst->eraseFromParent();
            }
        }
    }
    mOut << "DeadInstElimination: " << deadInstCount << " instructions removed\n";
    return PreservedAnalyses::all();
}


