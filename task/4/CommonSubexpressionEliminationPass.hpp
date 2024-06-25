#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>

// 公共子表达式消除
class CommonSubexpressionEliminationPass : public llvm::PassInfoMixin<CommonSubexpressionEliminationPass> {
public:
  explicit CommonSubexpressionEliminationPass(llvm::raw_ostream &out) : mOut(out) {}

  llvm::PreservedAnalyses run(llvm::Module &mod, llvm::ModuleAnalysisManager &mam);

private:
    llvm::raw_ostream &mOut;
};