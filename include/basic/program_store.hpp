#pragma once

#include <cstdint>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "basic/source_line.hpp"

namespace basic {

class ProgramStore {
public:
    void upsert(std::int32_t line_number, const std::string& source);
    void erase(std::int32_t line_number);
    bool contains(std::int32_t line_number) const;
    std::vector<SourceLine> ordered_lines() const;
    void list(std::ostream& output) const;
    void clear();

private:
    std::map<std::int32_t, std::string> lines_;
};

}  // namespace basic
