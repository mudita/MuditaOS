// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>
#include <SimInsertedState.hpp>

namespace at
{
    namespace qcfg::usbnet
    {
        constexpr auto tokensCount = 1;
        enum class Net
        {
            RMNET = 0,
            ECM,
            MBIM,
            RNDIS
        };
    } // namespace qcfg::usbnet

    namespace result
    {
        struct QCFGUsbnet : public Result
        {
            qcfg::usbnet::Net net;
            explicit QCFGUsbnet(const Result &that);
        };
    } // namespace result

    namespace cmd
    {
        class QCFGUsbnet : public Cmd
        {
          public:
            QCFGUsbnet() noexcept;
            explicit QCFGUsbnet(at::cmd::Modifier mod) noexcept;

            [[nodiscard]] auto parseQCFGUsbnet(const Result &base_result) -> result::QCFGUsbnet;
            void set(qcfg::usbnet::Net net);
        };
    } // namespace cmd

} // namespace at
