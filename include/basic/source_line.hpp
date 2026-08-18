#pragma once

#include <cstdint>
#include <string>

namespace basic {

struct SourceLine {
    std::int32_t line_number{0};
    std::string source;
};

}  // namespace basic
