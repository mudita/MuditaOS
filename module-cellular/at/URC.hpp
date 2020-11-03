// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/Utils.hpp>
#include "URCHandler.hpp"

namespace at::urc

{
    class URC
    {
      public:
        /**
         * Parses URC body and constructs an instance.
         * @param urcBody - URC message body without the header
         * @param urcHead - URC message head
         * @param tokenDelimiter - sign that separates parameters in URC message
         */
        URC(const std::string &urcBody, const std::string &urcHead = std::string(), char tokenDelimiter = ',');

        virtual ~URC() = default;

        /**
         * Checks weather URC message is valid. Should be overridden in derived class of concrete URC.
         * @return true if valid, false otherwise
         */
        virtual auto isValid() const noexcept -> bool
        {
            return !urcBody.empty() || !urcHead.empty();
        }

        /**
         * Gets vector of strings that represent URC parameters.
         * @return vector or parameters in order of appearance in message
         */
        auto getTokens() const -> std::vector<std::string>;

        /**
         * Gets URC body stripped of urc header.
         * @return URC body string
         */
        auto getUrcBody() const -> std::string
        {
            return urcBody;
        }

        /**
         * Call for dispatching handling to URCHandler
         * @param h - implementation of URCHandler
         */
        virtual void Handle(URCHandler &h)
        {}

        /**
         * Flag URC handled/unhandled
         * @param state - true for handled, false for unhandled
         */
        void setHandled(bool state)
        {
            isUrcHandled = state;
        }

        /**
         * @return true if URC has been flagged as handled, false otherwise
         */
        bool isHandled()
        {
            return isUrcHandled;
        }

      protected:
        std::vector<std::string> tokens;
        std::string urcBody;
        std::string urcHead;

        bool isUrcHandled = false;

        /**
         * Splits URC into head and tokenized data, cleans tokens from whitespaces and quotes
         * @param str - string to be split
         * @param tokenDelimiter - sign that separates parameters in URC message
         */
        void split(const std::string &str, char tokenDelimiter = ',');
    };

} // namespace at::urc
