#pragma once

#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/IRBuilder.h>

class PostDominatorTree : public llvm::AnalysisInfoMixin<PostDominatorTree>
{
    public:
        using Result = llvm::MapVector<const llvm::BasicBlock*, llvm::SmallPtrSet<const llvm::BasicBlock*, 4>>;
        Result run(llvm::Function& func, llvm::FunctionAnalysisManager& fam);
    
    private:
        static llvm::AnalysisKey Key;
        friend struct llvm::AnalysisInfoMixin<PostDominatorTree>;
};