#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>

class AlgebraicIdentityPass : public llvm::PassInfoMixin<AlgebraicIdentityPass>
{
    public:
        explicit AlgebraicIdentityPass(llvm::raw_ostream& out)
            : mOut(out)
        {
        }

        llvm::PreservedAnalyses run(llvm::Module& mod,
                                    llvm::ModuleAnalysisManager& mam);

    private:
        llvm::raw_ostream& mOut;
};