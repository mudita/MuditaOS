// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Cmd.hpp>

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

    Result &Cmd::parse(Result &that)
    {
        result = std::unique_ptr<Result>();
        return *result;
    }
} // namespace at
