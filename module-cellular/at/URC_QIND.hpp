#pragma once

#include "URC_Any.hpp"

namespace at::urc
{
    struct QIND : public Any
    {
        QIND(const std::string &val);
        ~QIND() override = default;
        auto what() -> std::string final;
        enum CSQ
        {
            CSQ,
            Val1,
            Val2
        };

        auto is_csq() -> bool;
        auto csq_val() -> std::pair<int, int>;
        /// by docs ivalid csq is 99,99
        auto csq_ivalid() -> std::pair<int, int>;
    };
}; // namespace at::urc
