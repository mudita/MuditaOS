// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EnterNumberWindow.hpp"

namespace gui
{

    class EmergencyCallWindow : public EnterNumberWindow
    {
      public:
        EmergencyCallWindow(app::Application *app, app::EnterNumberWindowInterface *interface);

        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
