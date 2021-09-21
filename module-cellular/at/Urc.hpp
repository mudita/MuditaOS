// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Utils.hpp>
#include "UrcHandler.hpp"

namespace at::urc

{
    class Urc
    {
      public:
        /**
         * Parses Urc body and constructs an instance.
         * @param urcBody - Urc message body without the header
         * @param urcHead - Urc message head
         * @param tokenDelimiter - sign that separates parameters in Urc message
         */
        Urc(const std::string &urcBody, const std::string &urcHead = std::string());

        virtual ~Urc() = default;

        /**
         * Checks weather Urc message is valid. Should be overridden in derived class of concrete Urc.
         * @return true if valid, false otherwise
         */
        virtual auto isValid() const noexcept -> bool
        {
            return !urcBody.empty() || !urcHead.empty();
        }

        /**
         * Gets vector of strings that represent Urc parameters.
         * @return vector or parameters in order of appearance in message
         */
        auto getTokens() const -> std::vector<std::string>;

        /**
         * Gets Urc body stripped of urc header.
         * @return Urc body string
         */
        auto getUrcBody() const -> std::string
        {
            return urcBody;
        }

        /**
         * Call for dispatching handling to UrcHandler
         * @param h - implementation of UrcHandler
         */
        virtual void Handle(UrcHandler &h)
        {}

        /**
         * Flag Urc handled/unhandled
         * @param state - true for handled, false for unhandled
         */
        void setHandled(bool state)
        {
            isUrcHandled = state;
        }

        /**
         * @return true if Urc has been flagged as handled, false otherwise
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
         * Splits Urc into head and tokenized data, cleans tokens from whitespaces and quotes
         * @param str - string to be split
         */
        virtual void split(const std::string &str);
    };

} // namespace at::urc
