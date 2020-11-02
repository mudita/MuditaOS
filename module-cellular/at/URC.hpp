// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/Utils.hpp>

namespace at::urc
{
    /**
     * Abstraction for cellular URC message
     */
    class URC
    {
      public:
        /**
         * Instantiates concrete URC class on a base of URC message body
         * @param urcMessage - URC message to be parsed
         * @return pointer to URC
         */
        static std::shared_ptr<URC> Create(const std::string &urcMessage);

        /**
         * Parses URC body and constructs an instance
         * @param urcBody - URC message body without the header
         * @param tokenDelimiter - sign that separates parameters in URC message
         */
        URC(const std::string &urcBody, char tokenDelimiter = ',');
        virtual ~URC() = default;

        /**
         * Checks weather URC message is valid. Should be overridden in derived class of concrete URC
         * @return true if valid, false otherwise
         */
        virtual auto isValid() const noexcept -> bool
        {
            return true;
        };

        /**
         * Gets vector of strings that represent URC parameters
         * @return vector or parameters in order of appearance in message
         */
        auto getTokens() const -> std::vector<std::string>;

      protected:
        std::vector<std::string> tokens;

        /**
         * Splits URC into head and tokenized data, cleans tokens from whitespaces and quotes
         * @param str - string to be split
         * @param tokenDelimiter - sign that separates parameters in URC message
         */
        virtual void split(const std::string &str, char tokenDelimiter = ',') final;
    };

    /**
     * Attempts to casts URC base class to concrete URC
     * @tparam urcType - type of URC to be returned
     * @param urc - instance of ORC to be casted
     * @return nullptr upon failure, URC of type urcType upon success
     */
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
