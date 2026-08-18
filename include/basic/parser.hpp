#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "basic/lexer.hpp"
#include "basic/program_ast.hpp"
#include "basic/source_line.hpp"

namespace basic {

enum class InterpreterCommand {
    Run,
    List,
    Clear,
    Quit,
    Help
};

enum class ParsedInputKind {
    Empty,
    Command,
    ProgramLine,
    DeleteLine,
    ImmediateStatement
};

class ParsedInput {
public:
    ParsedInput();
    ~ParsedInput();
    ParsedInput(const ParsedInput&) = delete;
    ParsedInput& operator=(const ParsedInput&) = delete;
    ParsedInput(ParsedInput&& other) noexcept;
    ParsedInput& operator=(ParsedInput&& other) noexcept;

    static ParsedInput command(InterpreterCommand command);
    static ParsedInput program_line(std::int32_t line_number,
                                    std::string source);
    static ParsedInput delete_line(std::int32_t line_number);
    static ParsedInput immediate(Statement* statement);

    ParsedInputKind kind() const noexcept;
    InterpreterCommand command_value() const;
    std::int32_t line_number() const;
    const std::string& source() const noexcept;
    Statement* release_statement() noexcept;

private:
    ParsedInputKind kind_{ParsedInputKind::Empty};
    InterpreterCommand command_{InterpreterCommand::Help};
    std::int32_t line_number_{0};
    std::string source_;
    Statement* statement_{nullptr};  // Owns an immediate statement.
};

class Parser {
public:
    ParsedInput parse_input(const std::string& source) const;
    ProgramAst* parse_program(const std::vector<SourceLine>& lines) const;

private:
    Statement* parse_simple_statement(TokenStream& tokens,
                                      const std::string& source) const;
    Expression* parse_expression(TokenStream& tokens,
                                 int minimum_precedence = 0) const;
    Condition* parse_condition(TokenStream& tokens) const;
};

}  // namespace basic
