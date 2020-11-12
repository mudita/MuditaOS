// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLock.hpp"
#include <service-cellular/CellularMessage.hpp>

#include "Interface/SettingsRecord.hpp"
namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PinLockHandler
    {
        app::ApplicationDesktop *app = nullptr;
        const SettingsRecord &appSettings;

        void parseSimCard(CellularSimResponseMessage *msg);
        void parseSimState(CellularSimResponseMessage *msg);
        void parseAttemptsAndPinSize(CellularSimResponseMessage *msg);

        void handleScreenPin(const std::vector<unsigned int> &pin);
        void handleSimPinOrPuk(const std::vector<unsigned int> &pin);

      public:
        PinLockHandler(app::ApplicationDesktop *app, SettingsRecord &settings);
        void reloadScreenLock();
        auto handle(CellularSimResponseMessage *msg) -> bool;
        void handle(const std::vector<unsigned int> &pin);

        gui::PinLock lock;
    };
} // namespace gui
