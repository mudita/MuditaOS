// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Cmd.hpp"
#include <algorithm>

namespace at
{
    Cmd::Cmd(std::string cmd, cmd::Modifier mod, std::chrono::milliseconds timeout) noexcept
        : cmd(std::move(cmd)), timeout(timeout), mod(mod)
    {}

    Cmd::Cmd(std::string cmd, std::chrono::milliseconds timeout) noexcept : Cmd(cmd, cmd::Modifier::None, timeout)
    {}

    Cmd::Cmd(const Cmd &p) noexcept
    {
        *this = operator=(p);
    }

    Cmd::Cmd(Cmd &&p) noexcept
    {
        *this = operator=(p);
    }

    void Cmd::split(const std::string &str, Result &result)
    {
        constexpr char tokenDelimiter = ',';

        result.tokens.push_back(utils::split(str, tokenDelimiter));
        constexpr auto urcStringDelimiter = "\"";
        for (auto &arr : result.tokens) {
            for (auto &t : arr) {
                utils::findAndReplaceAll(t, urcStringDelimiter, "");
                t = utils::trim(t);
            }
        }
    }

    auto Cmd::operator=(const Cmd &p) noexcept -> Cmd &
    {
        if (&p == this) {
            return *this;
        }
        this->cmd     = p.cmd;
        this->timeout = p.timeout;
        this->mod     = p.mod;
        this->body    = p.body;
        return *this;
    }

    auto Cmd::operator=(Cmd &&p) noexcept -> Cmd &
    {
        if (&p == this) {
            return *this;
        }
        this->cmd     = std::move(p.cmd);
        this->timeout = std::move(p.timeout);
        this->mod     = std::move(p.mod);
        this->body    = std::move(p.body);
        return *this;
    }

    Result Cmd::parseBase(const Result &that)
    {
        auto result = that;
        if (result.code != Result::Code::OK) {
            return result;
        }
        else if (result.response.empty()) {
            LOG_ERROR("Can't parse - empty response");
            result.code = Result::Code::PARSING_ERROR;
            return result;
        }
        else if (result.response.size() == 1) {
            if (result.response[0] == "OK") {
                LOG_INFO("OK response");
                result.code = Result::Code::OK;
                return result;
            }
            LOG_ERROR("Can't parse - response not valid");
            result.code = Result::Code::ERROR;
            return result;
        }
        const char headDelimiter = ':';
        const auto atResponse    = result.response;
        const auto lastPosition  = atResponse.end() - 1;
        for (auto it = atResponse.begin(); it != lastPosition; it++) {
            auto prefixIter = std::find(it->begin(), it->end(), headDelimiter);
            auto head       = std::string(it->begin(), prefixIter);
            auto body       = std::string(prefixIter == it->end() ? it->begin() : prefixIter + 1, it->end());
            split(body, result);
        }

        return result;
    }
} // namespace at
