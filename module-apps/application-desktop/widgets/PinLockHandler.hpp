// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLock.hpp"
#include <service-cellular/CellularMessage.hpp>
#include <module-services/service-db/agents/settings/Settings.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PinLockHandler
    {
        app::ApplicationDesktop *app = nullptr;

        void parseSimCard(CellularSimResponseMessage *msg);
        void parseSimState(CellularSimResponseMessage *msg);
        void parseAttemptsAndPinSize(CellularSimResponseMessage *msg);

        void handleScreenPin(const std::vector<unsigned int> &pin);
        void handleSimPinOrPuk(const std::vector<unsigned int> &pin);

      public:
        explicit PinLockHandler(app::ApplicationDesktop *app);
        void reloadScreenLock();
        auto handle(CellularSimResponseMessage *msg) -> bool;
        void handle(const std::vector<unsigned int> &pin);

        gui::PinLock lock;

      private:
        void lockPassHashChanged(const std::string &name, std::optional<std::string> value);
        unsigned int lockPassHash = 0;
    };
} // namespace gui
