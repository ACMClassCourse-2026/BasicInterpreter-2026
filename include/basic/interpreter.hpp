#pragma once

#include <iosfwd>
#include <string>

#include "basic/parser.hpp"
#include "basic/program.hpp"

namespace basic {

class Interpreter {
public:
    Interpreter(std::istream& input, std::ostream& output);

    bool process_line(const std::string& line);

private:
    void print_help();

    Parser parser_;
    Program program_;
    std::ostream& output_;  // Borrows the output stream.
};

}  // namespace basic
