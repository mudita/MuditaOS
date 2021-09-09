// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Dialog.hpp"

namespace gui
{
    class TetheringNotificationPopup : public gui::DialogConfirm
    {
      public:
        TetheringNotificationPopup(app::ApplicationCommon *app, const std::string &name);

        gui::status_bar::Configuration configureStatusBar(gui::status_bar::Configuration appConfiguration) override;
    };
} // namespace gui
