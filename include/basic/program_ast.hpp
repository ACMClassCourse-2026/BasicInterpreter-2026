#pragma once

#include "basic/statement.hpp"

namespace basic {

class ProgramAst {
public:
    explicit ProgramAst(BlockStatement* root);
    ~ProgramAst();
    ProgramAst(const ProgramAst&) = delete;
    ProgramAst& operator=(const ProgramAst&) = delete;
    ProgramAst(ProgramAst&& other) noexcept;
    ProgramAst& operator=(ProgramAst&& other) noexcept;

    const BlockStatement& root() const;
    ExecSignal execute(ExecutionContext& context) const;

private:
    BlockStatement* root_;  // Owns the root block.
};

}  // namespace basic
