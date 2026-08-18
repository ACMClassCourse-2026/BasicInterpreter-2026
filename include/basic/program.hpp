#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>

#include "basic/environment.hpp"
#include "basic/parser.hpp"
#include "basic/program_store.hpp"

namespace basic {

class Program {
public:
    Program(std::istream& input, std::ostream& output);

    void upsert_line(std::int32_t line_number, const std::string& source);
    void erase_line(std::int32_t line_number);
    void list() const;
    void clear();
    void run();
    // Takes ownership of statement.
    void execute_immediate(Statement* statement);

private:
    ProgramStore store_;
    Environment environment_;
    Parser parser_;
    std::istream& input_;    // Borrows the input stream.
    std::ostream& output_;   // Borrows the output stream.
};

}  // namespace basic
