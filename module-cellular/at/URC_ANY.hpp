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
        Any(const std::string &str)
        {
            split(str);
        }
        virtual ~Any() = default;
        virtual auto what() -> std::string = 0;
        std::string head;
        std::vector<std::string> tokens;
        virtual void split(const std::string &str) final
        {
            const std::string delim = ": ";
            auto pos = str.find(delim);
            head = std::string(str, 0, pos);
            if (pos != std::string::npos)
            {
                tokens = utils::split(std::string(str.begin() + pos + delim.size(), str.end()), ',');
            } // else - everyting went to head
        }

        virtual auto is() -> bool final
        {
            return head.find(what());
        }

      private:
        Any() = default;
    };
}; // namespace at::urc
