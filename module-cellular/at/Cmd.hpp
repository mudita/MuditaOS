// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"

#include <at/Constants.hpp>

#include <string>
#include <chrono>

namespace at
{
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
        cmd::Modifier mod = cmd::Modifier::None; /// modifier responsible to define action we want to perform
        std::string body;                        /// part after command name
      public:
        Cmd() = delete;
        Cmd(std::string cmd, cmd::Modifier mod, std::chrono::milliseconds timeout = default_timeout) noexcept;
        Cmd(std::string cmd, std::chrono::milliseconds timeout = default_timeout) noexcept;
        Cmd(const Cmd &p) noexcept = default;
        Cmd(Cmd &&) noexcept       = default;
        auto operator=(const Cmd &) noexcept -> Cmd & = default;
        auto operator=(Cmd &&) noexcept -> Cmd & = default;
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

      protected:
        [[nodiscard]] static auto parseBase(const Result &base_result) -> Result;
        static void split(const std::string &str, Result &result);
    };
}; // namespace at
