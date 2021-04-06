// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Dialog.hpp"

namespace gui
{
    class TetheringNotificationPopup : public gui::DialogConfirm
    {
      public:
        TetheringNotificationPopup(app::Application *app, const std::string &name);

        gui::top_bar::Configuration configureTopBar(gui::top_bar::Configuration appConfiguration) override;
    };
} // namespace gui
