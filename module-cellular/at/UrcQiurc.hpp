// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

namespace at::urc
{

    /// +QIURC: <action>,[p1], [p2]  - multipurpose URC information (in general for TCP connection)
    class Qiurc : public Urc
    {
        /**
         * In general could be at last one param in +QIURC: "incoming full"
         * to such with CR LF messages contain data +QIURC:
         * "recv",<connectID>,<currentrecvlength>,<remoteIP>,<remote_port><CR><LF><data>
         */
        enum Tokens
        {
            Type       = 0,
            FirstParam = 1
        };

      public:
        enum class QIUrcMessages
        {
            DeactivateContext = 1
        };
        static constexpr auto qiurcPdpdeact      = "pdpdeact"; ///<  +QIURC:"pdpdeact",<contextID>
        static constexpr auto qiurcPdpdeactCount = 2;

        static constexpr auto head = "+QIURC";

        static bool isURC(const std::string &uHead)
        {
            return uHead.find(Qiurc::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;

        [[nodiscard]] auto getType() const noexcept -> std::optional<QIUrcMessages>;

        [[nodiscard]] auto getFirstParam() const noexcept -> std::optional<std::string>;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };

} // namespace at::urc
