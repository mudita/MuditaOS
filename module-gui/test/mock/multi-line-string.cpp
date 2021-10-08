// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "multi-line-string.hpp"
#include <ostream>
#include <sstream>

namespace mockup
{
    auto multiLineString(unsigned int no_lines) -> std::string
    {
        std::stringstream text;
        for (auto el : lineStrings(no_lines)) {
            text << el;
        }
        return text.str();
    }

    auto multiWordString(unsigned int no_words) -> std::string
    {
        std::stringstream text;
        for (unsigned int i = 0; i < no_words; ++i) {
            /// pre pend with `-`
            for (unsigned int j = 1; j < no_words; ++j) {
                text << "-";
            }
            /// `word:<no>`
            text << "word:" << i << " ";
        }
        return text.str();
    }

    auto lineStrings(unsigned int count) -> std::list<std::string>
    {
        std::list<std::string> list;
        for (unsigned int i = 0; i < count; ++i) {
            std::stringstream text;
            text << "line:" << i << std::endl;
            list.push_back(text.str());
        }
        return list;
    }
}; // namespace mockup
