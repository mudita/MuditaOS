#pragma once

#include <list>
#include <string>

namespace mockup
{
    auto multiLineString(unsigned int no_lines) -> std::string;

    auto multiWordString(unsigned int no_words) -> std::string;

    auto lineStrings(unsigned int count) -> std::list<std::string>;

} // namespace mockup
