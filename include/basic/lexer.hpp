#pragma once

#include <string>

#include "basic/token.hpp"

namespace basic {

class Lexer {
public:
    TokenStream tokenize(const std::string& source) const;
};

}  // namespace basic
