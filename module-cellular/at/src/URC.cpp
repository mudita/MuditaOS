// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC.hpp>
#include <URC_CREG.hpp>
#include <URC_CTZE.hpp>
#include <URC_CUSD.hpp>
#include <URC_QIND.hpp>
#include <URC_CMTI.hpp>
#include <URC_CLIP.hpp>
#include <algorithm>

namespace at::urc
{
    std::shared_ptr<URC> URC::Create(const std::string &urcMessage)
    {
        if (urcMessage.empty()) {
            return std::make_unique<URC>(std::string());
        }

        const char headDelimiter = ':';
        auto it                  = std::find(urcMessage.begin(), urcMessage.end(), headDelimiter);
        std::string head         = std::string(urcMessage.begin(), it);
        std::string body         = std::string(it == urcMessage.end() ? it : it + 1, urcMessage.end());

        if (head.find(CTZE::head) != std::string::npos) {
            return std::make_unique<CTZE>(body);
        }
        else if (head.find(CREG::head) != std::string::npos) {
            return std::make_unique<CREG>(body);
        }
        else if (head.find(QIND::head) != std::string::npos) {
            return std::make_unique<QIND>(body);
        }
        else if (head.find(CUSD::head) != std::string::npos) {
            return std::make_unique<CUSD>(body);
        }
        else if (head.find(CMTI::head) != std::string::npos) {
            return std::make_unique<CMTI>(body);
        }
        else if (head.find(CLIP::head) != std::string::npos) {
            return std::make_unique<CLIP>(body);
        }

        return std::make_unique<URC>(std::string());
    }

    URC::URC(const std::string &urcBody, char tokenDelimiter)
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
