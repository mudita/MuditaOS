// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <gui/widgets/Label.hpp>
#include <Translator.hpp>
#include <notifications/NotificationsModel.hpp>
#include <ListView.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PhoneLockedWindow : public AppWindow
    {
      protected:
        gui::Label *time                                            = nullptr;
        gui::Label *dayText                                         = nullptr;
        gui::ListView *notificationsList                            = nullptr;
        std::shared_ptr<gui::NotificationsModel> notificationsModel = nullptr;

        bool processLongReleaseEvent(const InputEvent &inputEvent);

      public:
        PhoneLockedWindow(app::ApplicationCommon *app, const std::string &name);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        bool updateTime() override;
    };

} /* namespace gui */
