// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "NumberWindow.hpp"

namespace gui
{
    class EmergencyCallWindow : public NumberWindow
    {
      public:
        EmergencyCallWindow(app::ApplicationCommon *app, app::EnterNumberWindowInterface *interface);
        ~EmergencyCallWindow() override = default;

        bool onInput(const InputEvent &inputEvent) override;

        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
