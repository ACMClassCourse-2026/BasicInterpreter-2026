#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "basic/condition.hpp"
#include "basic/execution_context.hpp"
#include "basic/value.hpp"

namespace basic {

enum class ExecSignal {
    Continue,
    EndProgram,
    ExitFor
};

class AssignmentTarget {
public:
    AssignmentTarget(std::string name, std::optional<BasicType> suffix_type,
                     std::vector<Expression*> indices);
    ~AssignmentTarget();
    AssignmentTarget(const AssignmentTarget&) = delete;
    AssignmentTarget& operator=(const AssignmentTarget&) = delete;
    AssignmentTarget(AssignmentTarget&& other) noexcept;
    AssignmentTarget& operator=(AssignmentTarget&& other) noexcept;

    const std::string& name() const noexcept;
    std::optional<BasicType> suffix_type() const noexcept;
    const std::vector<Expression*>& indices() const noexcept;

private:
    void clear() noexcept;

    std::string name_;
    std::optional<BasicType> suffix_type_;
    std::vector<Expression*> indices_;  // Owns all index expressions.
};

class Statement {
public:
    explicit Statement(std::string source);
    virtual ~Statement() = default;
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    virtual ExecSignal execute(ExecutionContext& context) const = 0;
    const std::string& source() const noexcept;

private:
    std::string source_;
};

class RemStatement final : public Statement {
public:
    using Statement::Statement;
    ExecSignal execute(ExecutionContext& context) const override;
};

class DimStatement final : public Statement {
public:
    DimStatement(std::string source, std::string name,
                 std::optional<BasicType> declared_type,
                 std::vector<Expression*> dimensions, bool dynamic,
                 Expression* initializer);
    ~DimStatement() override;

    ExecSignal execute(ExecutionContext& context) const override;

private:
    std::string name_;
    std::optional<BasicType> declared_type_;
    std::vector<Expression*> dimensions_;  // Owns all dimensions.
    bool dynamic_{false};
    Expression* initializer_;  // Owns the optional initializer.
};

class LetStatement final : public Statement {
public:
    LetStatement(std::string source, AssignmentTarget target,
                 Expression* expression);
    ~LetStatement() override;

    ExecSignal execute(ExecutionContext& context) const override;

private:
    AssignmentTarget target_;
    Expression* expression_;  // Owns the expression.
};

class PrintStatement final : public Statement {
public:
    PrintStatement(std::string source, Expression* expression);
    ~PrintStatement() override;

    ExecSignal execute(ExecutionContext& context) const override;
    const Expression& expression() const noexcept;

private:
    Expression* expression_;  // Owns the expression.
};

class InputStatement final : public Statement {
public:
    InputStatement(std::string source, AssignmentTarget target);
    ExecSignal execute(ExecutionContext& context) const override;

private:
    AssignmentTarget target_;
};

class EndStatement final : public Statement {
public:
    using Statement::Statement;
    ExecSignal execute(ExecutionContext& context) const override;
};

class RedimStatement final : public Statement {
public:
    RedimStatement(std::string source, std::string name, Expression* length,
                   bool preserve);
    ~RedimStatement() override;

    ExecSignal execute(ExecutionContext& context) const override;

private:
    std::string name_;
    Expression* length_;  // Owns the length expression.
    bool preserve_{false};
};

class EraseStatement final : public Statement {
public:
    EraseStatement(std::string source, std::string name);
    ExecSignal execute(ExecutionContext& context) const override;

private:
    std::string name_;
};

class BlockStatement final : public Statement {
public:
    BlockStatement();
    explicit BlockStatement(std::vector<Statement*> statements);
    ~BlockStatement() override;
    BlockStatement(const BlockStatement&) = delete;
    BlockStatement& operator=(const BlockStatement&) = delete;

    void append(Statement* statement);
    const std::vector<Statement*>& statements() const noexcept;
    ExecSignal execute(ExecutionContext& context) const override;

private:
    std::vector<Statement*> statements_;  // Owns all statements.
};

struct IfBranch {
    Condition* condition{nullptr};  // Owns the optional condition.
    BlockStatement* body{nullptr};  // Owns the branch body.
};

class IfStatement final : public Statement {
public:
    IfStatement(std::string source, std::vector<IfBranch> branches);
    ~IfStatement() override;
    IfStatement(const IfStatement&) = delete;
    IfStatement& operator=(const IfStatement&) = delete;

    ExecSignal execute(ExecutionContext& context) const override;

private:
    std::vector<IfBranch> branches_;
};

class ForStatement final : public Statement {
public:
    ForStatement(std::string source, std::string variable,
                 Expression* start, Expression* end, Expression* step,
                 BlockStatement* body);
    ~ForStatement() override;

    ExecSignal execute(ExecutionContext& context) const override;

private:
    std::string variable_;
    Expression* start_;      // Owns the start expression.
    Expression* end_;        // Owns the end expression.
    Expression* step_;       // Owns the optional step expression.
    BlockStatement* body_;   // Owns the loop body.
};

class WhileStatement final : public Statement {
public:
    WhileStatement(std::string source, Condition* condition,
                   BlockStatement* body);
    ~WhileStatement() override;

    ExecSignal execute(ExecutionContext& context) const override;

private:
    Condition* condition_;  // Owns the condition.
    BlockStatement* body_;  // Owns the loop body.
};

class ExitForStatement final : public Statement {
public:
    using Statement::Statement;
    ExecSignal execute(ExecutionContext& context) const override;
};

}  // namespace basic
