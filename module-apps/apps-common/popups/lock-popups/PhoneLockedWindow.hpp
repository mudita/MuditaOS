// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/presenter/WallpaperPresenter.hpp>
#include <AppWindow.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PhoneLockedWindow : public AppWindow
    {
      public:
        PhoneLockedWindow(app::ApplicationCommon *app,
                          const std::string &name,
                          std::unique_ptr<WallpaperPresenter> &&presenter,
                          unsigned lockScreenDeepRefreshRate);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        RefreshModes updateTime() override;

      private:
        bool processLongReleaseEvent(const InputEvent &inputEvent);

        std::unique_ptr<WallpaperPresenter> wallpaperPresenter;
        bool refreshedOnPhoneLockTimeLock = false;

        void initializeDeepRefreshCounter(unsigned deepRefreshRate);

        struct
        {
            unsigned currentValue;
            unsigned referenceValue;

            [[nodiscard]] inline auto overflow() const noexcept -> bool
            {
                return currentValue >= referenceValue;
            }

            inline auto reset() noexcept -> void
            {
                currentValue = 0;
            }

            inline auto setReference(unsigned reference) noexcept -> void
            {
                referenceValue = reference;
            }

            inline auto operator++(int) noexcept -> unsigned
            {
                auto temp = currentValue;
                currentValue++;
                return temp;
            }
        } deepRefreshCounter;

        void updateStatusBar();
    };

} /* namespace gui */
