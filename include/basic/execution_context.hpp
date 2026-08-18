#pragma once

#include <iosfwd>

namespace basic {

class Environment;

class ExecutionContext {
public:
    ExecutionContext(Environment& environment, std::istream& input,
                     std::ostream& output);

    Environment& environment();
    const Environment& environment() const;
    std::istream& input();
    std::ostream& output();

private:
    Environment& environment_;  // Borrows the execution environment.
    std::istream& input_;        // Borrows the input stream.
    std::ostream& output_;       // Borrows the output stream.
};

}  // namespace basic
