#include "DeadInstEliminationPass.hpp"
#include <queue>
#include <set>

using namespace llvm;

PreservedAnalyses
DeadInstEliminationPass::run(Module& mod, ModuleAnalysisManager& MAM)
{
  unsigned removedInstructions = 0;

  std::set<Instruction*> toBeRemoved;
  for (Function& Func : mod) {
    for (BasicBlock& Bloc : Func) {
      for (Instruction& Inst : Bloc) {
        if (AllocaInst* AI = dyn_cast<AllocaInst>(&Inst)) {
          if (AI->user_empty()) {
            toBeRemoved.insert(AI);
          }
        } else if (StoreInst* SI = dyn_cast<StoreInst>(&Inst)) {
          Value* Ptr = SI->getPointerOperand();

          if (Ptr->hasOneUser() && !isa<GetElementPtrInst>(Ptr)) {
            toBeRemoved.insert(SI);

            std::queue<Value*> explorationQueue;
            explorationQueue.push(SI->getValueOperand());
            while (!explorationQueue.empty()) {
              Value* CurrentVal = explorationQueue.front();
              explorationQueue.pop();

              for (User* UserInst : CurrentVal->users()) {
                if (Instruction* Instr = dyn_cast<Instruction>(UserInst)) {
                  toBeRemoved.insert(Instr);
                  explorationQueue.push(Instr);
                }
              }
            }
          }
        }
      }
    }
  }

  for (Instruction* Instr : toBeRemoved) {
    Instr->eraseFromParent();
    ++removedInstructions;
  }
  mOut << "DeadInstructionRemoval: Processed module, removed "
       << removedInstructions << " instructions.\n";
  return PreservedAnalyses::all();
}