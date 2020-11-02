// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Utils.hpp"
#include <log/log.hpp>
#include <string>
#include <vector>
#include <iostream>

namespace at::urc
{
    class URC
    {
      public:
        static std::shared_ptr<URC> Create(const std::string &urcBody);

        URC(const std::string &str, char tokenDelimiter = ',');
        virtual ~URC() = default;

        virtual auto isValid() const noexcept -> bool
        {
            return true;
        };
        auto getTokens() const -> std::vector<std::string>;

      protected:
        const std::string body;
        std::vector<std::string> tokens;
        /// split urc into: head and tokenized data (as in class description)
        virtual void split(const std::string &str, char tokenDelimiter = ',') final;
    };

    template <typename urcType> static auto getURC(std::shared_ptr<URC> urc) -> std::shared_ptr<urcType>
    {
        if (urc) {
            auto &rawUrc = *urc.get();
            if (typeid(rawUrc) == typeid(urcType)) {
                return std::static_pointer_cast<urcType>(urc);
            }
        }

        return nullptr;
    }
}; // namespace at::urc
