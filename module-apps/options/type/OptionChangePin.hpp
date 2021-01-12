// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Option.hpp"
#include "OptionBase.hpp"
#include "OptionStyle.hpp"
#include "Application.hpp"

#include "Text.hpp"

#include "module-apps/application-settings/ApplicationSettings.hpp"

namespace gui
{
    class PinChange : public gui::Text
    {};

} // namespace gui

namespace gui::option
{
    class ChangePin : public Base
    {
      private:
        app::Application *app      = nullptr;
        app::PinLockSetter *setter = nullptr;
        unsigned int lockHash      = 0;

      public:
        ChangePin(app::Application *app, app::PinLockSetter *setter, unsigned int lockHash)
            : app(app), setter(setter), lockHash(lockHash)
        {}

        void setLockHash(unsigned int hash)
        {
            lockHash = hash;
        }

        [[nodiscard]] auto build() const -> ListItem * override;
    };
} // namespace gui::option

std::list<gui::Option> settingChangePinWindow(app::Application *app, app::PinLockSetter *setter, unsigned int lockHash);
