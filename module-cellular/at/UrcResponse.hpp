// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

namespace at::urc
{
    class UrcResponse : public Urc
    {
      public:
        // this table has to be aligned with responseBodies
        enum class URCResponseType
        {
            Ok,
            Connect,
            Ring,
            NoCarrier,
            Error,
            NoDialtone,
            Busy,
            NoAnswer
        };

        static auto isURC(const std::string &uHead) -> std::optional<URCResponseType>
        {
            for (auto &resp : urcResponses) {
                if (uHead.find(resp.second) != std::string::npos) {
                    return resp.first;
                }
            }
            return std::nullopt;
        }

        explicit UrcResponse(URCResponseType type) : Urc(std::string()), type(type){};

        auto getURCResponseType() const noexcept -> URCResponseType;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }

      private:
        URCResponseType type;

        static constexpr std::array<std::pair<URCResponseType, std::string_view>, 8> urcResponses = {{
            {URCResponseType::Ok, "OK"},
            {URCResponseType::Connect, "CONNECT"},
            {URCResponseType::Ring, "RING"},
            {URCResponseType::NoCarrier, "NO CARRIER"},
            {URCResponseType::Error, "ERROR"},
            {URCResponseType::NoDialtone, "NO DIALTONE"},
            {URCResponseType::Busy, "BUSY"},
            {URCResponseType::NoAnswer, "NO ANSWER"},
        }};
    };
} // namespace at::urc
