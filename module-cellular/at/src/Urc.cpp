// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Urc.hpp>
#include <algorithm>

namespace at::urc
{
    Urc::Urc(const std::string &urcBody, const std::string &urcHead) : urcBody(utils::trim(urcBody)), urcHead(urcHead)
    {
        split(urcBody);
    }

    void Urc::split(const std::string &str)
    {
        constexpr char tokenDelimiter = ',';

        tokens                            = utils::split(str, tokenDelimiter);
        constexpr auto urcStringDelimiter = "\"";
        for (auto &t : tokens) {
            utils::findAndReplaceAll(t, urcStringDelimiter, "");
            t = utils::trim(t);
        }
    }

    auto Urc::getTokens() const -> std::vector<std::string>
    {
        return tokens;
    }
} // namespace at::urc
