// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC_Any.hpp>

namespace at::urc
{

    Any::Any(const std::string &str, char tokenDelimiter)
    {
        split(str, tokenDelimiter);
    }

    void Any::split(const std::string &str, char tokenDelimiter)
    {
        const std::string delim = ":";
        auto pos                = str.find(delim);
        head                    = std::string(str, 0, pos);
        if (pos != std::string::npos) {
            tokens = utils::split(std::string(str.begin() + pos + delim.size(), str.end()), tokenDelimiter);
        } // else - everyting went to head
    }

    auto Any::is() const -> bool
    {
        return head.find(what()) != std::string::npos;
    }

    auto Any::getTokens() const -> std::vector<std::string>
    {
        return tokens;
    }

} // namespace at::urc
