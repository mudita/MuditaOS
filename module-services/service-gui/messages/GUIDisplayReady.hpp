// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

namespace service::gui
{
    class GUIDisplayReady : public sgui::GUIMessage
    {

        bool suspendInProgress  = false;
        bool shutdownInProgress = false;

      public:
        GUIDisplayReady(bool suspendInProgress, bool shutdownInProgress)
            : suspendInProgress(suspendInProgress), shutdownInProgress(shutdownInProgress)
        {}

        [[nodiscard]] auto getSuspendInProgress() const noexcept
        {
            return suspendInProgress;
        }

        [[nodiscard]] auto getShutdownInProgress() const noexcept
        {
            return shutdownInProgress;
        }
    };
} // namespace service::gui
