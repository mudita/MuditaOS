// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <Text.hpp>

namespace gui
{
    namespace window
    {
        namespace name
        {
            inline constexpr auto ussd_window = "ussd_window";
        } // namespace name
    }     // namespace window
    class USSDWindow : public AppWindow
    {
      protected:
        gui::Label *incomingLabel = nullptr;
        gui::Label *outgoingLabel = nullptr;

        gui::Text *pullToSend = nullptr;
        gui::Text *received   = nullptr;

        gui::VBox *box = nullptr;

        gui::Label *addTitle(const UTF8 &title);
        gui::Text *addText(void);

      public:
        USSDWindow(app::Application *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void set_navigation();
        void handleIncomingUSSD(const std::string &data);
    };
} // namespace gui
