#include "PostDominatorTree.hpp"

#include <llvm/IR/CFG.h>
#include <set>

using namespace llvm;

PostDominatorTree::Result PostDominatorTree::run(Function& func, FunctionAnalysisManager& fam) {
    Result result;

    // 初始化后支配集合
    for (auto& bb : func) {
        for (auto& b:func)
            result[&bb].insert(&b);
    }

    // 迭代计算后支配关系
    bool changed;
    do {
        changed = false;
        for (auto& bb : func) {
            SmallPtrSet<const BasicBlock*, 4> new_set;
            new_set.insert(&bb);

            for (auto* succ : successors(&bb)) {
                new_set.insert(result[succ].begin(), result[succ].end());
            }

            if (new_set != result[&bb]) {
                result[&bb] = std::move(new_set);
                changed = true;
            }
        }
    } while (changed);

    // 构建后支配树
    for (auto& bb : func) {
        for (auto* succ : successors(&bb)) {
            if (result[&bb].count(succ) == 0) {
                result[&bb].erase(succ);
            }
        }
    }

    return result;
}

AnalysisKey PostDominatorTree::Key;