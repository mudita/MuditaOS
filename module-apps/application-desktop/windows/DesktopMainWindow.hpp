// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-desktop/models/ActiveNotificationsModel.hpp>

#include <AppWindow.hpp>
#include <gui/widgets/Label.hpp>
#include <Translator.hpp>
#include <ListView.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class NotificationsModel;

    class DesktopMainWindow : public AppWindow
    {
      protected:
        gui::Label *time    = nullptr;
        gui::Label *dayText = nullptr;

        gui::ListView *notificationsList                                  = nullptr;
        std::shared_ptr<gui::ActiveNotificationsModel> notificationsModel = nullptr;

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState();
        void setActiveState();
        bool processLongReleaseEvent(const InputEvent &inputEvent);
        bool processShortReleaseEvent(const InputEvent &inputEvent);
        app::ApplicationDesktop *getAppDesktop() const;

      public:
        explicit DesktopMainWindow(app::Application *app);

        // virtual methods gui::Window
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        // virtual methods gui::AppWindow
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;

        bool updateTime() override;

      private:
        void invalidate() noexcept;
        std::string osUpdateVer;
        std::string osCurrentVer;

        gui::KeyInputMappedTranslation translator;
    };

} /* namespace gui */
