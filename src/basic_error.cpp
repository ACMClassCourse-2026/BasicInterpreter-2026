#include "basic/basic_error.hpp"

namespace basic {

const char* error_message(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Syntax:
            return "SYNTAX ERROR";
        case ErrorCode::DivideByZero:
            return "DIVIDE BY ZERO";
        case ErrorCode::InvalidNumber:
            return "INVALID NUMBER";
        case ErrorCode::VariableNotDefined:
            return "VARIABLE NOT DEFINED";
        case ErrorCode::VariableAlreadyDefined:
            return "VARIABLE ALREADY DEFINED";
        case ErrorCode::TypeMismatch:
            return "TYPE MISMATCH";
        case ErrorCode::Overflow:
            return "OVERFLOW";
        case ErrorCode::InvalidArraySize:
            return "INVALID ARRAY SIZE";
        case ErrorCode::ArrayNotDimensioned:
            return "ARRAY NOT DIMENSIONED";
        case ErrorCode::SubscriptOutOfRange:
            return "SUBSCRIPT OUT OF RANGE";
        case ErrorCode::InvalidRedim:
            return "INVALID REDIM";
        case ErrorCode::StepCannotBeZero:
            return "STEP CANNOT BE ZERO";
    }
    return "SYNTAX ERROR";
}

BasicError::BasicError(ErrorCode code)
    : std::runtime_error(error_message(code)), code_(code) {}

BasicError::BasicError(const std::string& message)
    : std::runtime_error(message), code_(ErrorCode::Syntax) {}

ErrorCode BasicError::code() const noexcept { return code_; }

[[noreturn]] void todo(const char* function_name) {
    throw std::logic_error(std::string("TODO: ") + function_name);
}

}  // namespace basic
