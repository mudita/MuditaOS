// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NumberWindow.hpp"

namespace gui
{
    class EnterNumberWindow : public NumberWindow
    {
      private:
        gui::AddContactIcon *newContactIcon = nullptr;

      public:
        EnterNumberWindow(app::ApplicationCommon *app,
                          app::EnterNumberWindowInterface *interface,
                          std::string windowName = gui::window::name::enter_number);
        ~EnterNumberWindow() override = default;

        auto handleSwitchData(SwitchData *data) -> bool override;

        void buildInterface() override;

        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        auto addNewContact() -> bool;
    };
} /* namespace gui */
