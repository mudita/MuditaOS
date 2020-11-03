// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class URC_RESPONSE : public URC
    {
        // this enum has to be aligned with URCResponseType
        static constexpr const char *responseBodies[] = {
            "OK", "CONNECT", "RING", "NO CARRIER", "ERROR", "NO DIALTONE", "BUSY", "NO ANSWER"};

      public:
        // this table has to be aligned with responseBodies
        enum class URCResponseType
        {
            OK,
            CONNECT,
            RING,
            NO_CARRIER,
            ERROR,
            NO_DIALTONE,
            BUSY,
            NO_ANSWER
        };

        /**
         * Create URC_RESPONSE on a basis of URC response string
         * @param body - URC response body to be parsed
         * @return nullptr upon failure, URC_RESPONSE pointer upon success
         */
        static std::unique_ptr<URC_RESPONSE> Create(const std::string body);

        explicit URC_RESPONSE(URCResponseType type) : URC(std::string()), type(type){};

        URCResponseType getURCResponseType() const noexcept;

        void Handle(URCHandler &h) final
        {
            h.Handle(*this);
        }

      private:
        URCResponseType type;
    };
} // namespace at::urc
