// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC.hpp>
#include <algorithm>

namespace at::urc
{
    URC::URC(const std::string &urcBody, const std::string &urcHead, char tokenDelimiter)
        : urcBody(urcBody), urcHead(urcHead)
    {
        split(urcBody, tokenDelimiter);
    }

    void URC::split(const std::string &str, char tokenDelimiter)
    {
        tokens                            = utils::split(str, tokenDelimiter);
        constexpr auto urcStringDelimiter = "\"";
        for (auto &t : tokens) {
            utils::findAndReplaceAll(t, urcStringDelimiter, "");
            t = utils::trim(t);
        }
    }

    auto URC::getTokens() const -> std::vector<std::string>
    {
        return tokens;
    }
} // namespace at::urc
