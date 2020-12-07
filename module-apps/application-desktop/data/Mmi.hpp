// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace mmi
{
    static inline std::string removePhrase(std::string str, std::string phrase)
    {
        auto find_pos = str.find(phrase);
        while (find_pos != std::string::npos) {
            str.replace(find_pos, phrase.size(), "");
            find_pos = str.find(phrase, find_pos);
        }
        return str;
    }
} // namespace mmi
