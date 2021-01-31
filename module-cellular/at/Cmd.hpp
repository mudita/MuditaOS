// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <Result.hpp>
#include <chrono>

namespace at
{
    using namespace std::chrono_literals;

    struct Cmd;

    /// if you've checked it's ok - or it was at least 300 in code somewhere, take this
    const auto default_timeout = 300ms;

    namespace cmd
    {
        enum class Modifier
        {
            None,
            Get,
            Set
        };
    }

    /// at::Cmd structure with command, it's timeout and some runtime data
    /// { command, timeout, last : {sent, response, status } }
    struct Cmd
    {
      private:
        std::string cmd;                                     /// command head to run (AT, CLCC etc...)
        std::chrono::milliseconds timeout = default_timeout; /// timeout for this command
      protected:
        std::unique_ptr<Result> result;          /// lifetime result storage to be able to return reference to it
        cmd::Modifier mod = cmd::Modifier::None; /// modifier responsible to define action we want to perform
        std::string body;                        /// part after command name
      public:
        Cmd() = delete;
        Cmd(std::string cmd, cmd::Modifier mod, std::chrono::milliseconds timeout = default_timeout) noexcept;
        Cmd(std::string cmd, std::chrono::milliseconds timeout = default_timeout) noexcept;
        Cmd(const Cmd &p) noexcept;
        Cmd(Cmd &&) noexcept;
        auto operator  =(const Cmd &) noexcept -> Cmd &;
        auto operator  =(Cmd &&) noexcept -> Cmd &;
        virtual ~Cmd() = default;

        auto setModifier(cmd::Modifier mod) noexcept
        {
            this->mod = mod;
        }

        [[nodiscard]] auto getCmd() const
        {
            switch (mod) {
            case cmd::Modifier::Get:
                return cmd + "?";
            case cmd::Modifier::Set:
                return cmd + "=" + body;
            case cmd::Modifier::None:
                return cmd;
            }
            return cmd;
        }

        [[nodiscard]] auto getTimeout() const noexcept
        {
            return timeout;
        }

        /// not the prettiest, for now it's ok - for commands which modify strings to execute - return copy of command
        /// str
        [[nodiscard]] operator std::string() const
        {
            return getCmd();
        }

        auto operator+(const std::string &val) const -> Cmd
        {
            Cmd tmp = *this;
            tmp.cmd += val;
            return tmp;
        }

        /// would have used optional but it doesn't work with references
        [[nodiscard]] virtual auto parse(Result &base_result) -> Result &;
    };
}; // namespace at
