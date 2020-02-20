#pragma once

#include "Utils.hpp"
#include <log/log.hpp>
#include <string>
#include <vector>

namespace at::urc
{

    /// generally URC are like that: re'*([+^][A-Z]): (([A-Z],)+)' where first group is `head` and rest is tokens
    /// i.e. in:
    /// +QIND: "csq",15,99
    /// '+QIND' - head
    /// '"csq", 15, 99' are three tokens
    struct Any
    {
        Any(const std::string &str);
        virtual ~Any() = default;
        virtual auto what() -> std::string = 0;
        /// split urc into: head and tokenized data (as in class descripiton)
        virtual void split(const std::string &str) final;
        /// check if urc parsed is of proper type - i.e. QIND
        virtual auto is() -> bool final;

        std::string head;
        std::vector<std::string> tokens;

      private:
        Any() = default;
    };
}; // namespace at::urc
