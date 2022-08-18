// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include <sstream>

namespace utils
{
    template <typename Out>
    void split(const std::string &s, char delim, Out result)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    static inline std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    static inline std::vector<std::string> split(const std::string &s,
                                                 const std::string &delimiter,
                                                 const bool skipEmptyTokens = true)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;
        uint32_t tokenCount = 0;

        while (((pos_end = s.find(delimiter, pos_start)) != std::string::npos)) {
            token     = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            if (!skipEmptyTokens || !token.empty()) {
                tokenCount++;
                res.push_back(token);
            }
        }

        token = s.substr(pos_start);
        if (!skipEmptyTokens || !token.empty()) {
            res.push_back(token);
        }
        return res;
    }
} // namespace utils
