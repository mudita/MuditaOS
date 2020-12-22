// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "widgets/PinLockBox.hpp"
#include "widgets/PinLockBaseWindow.hpp"
namespace gui
{
    class PinLockWindow : public PinLockBaseWindow
    {
        const std::string this_window_name;
        std::string lockTimeoutApplication  = "";
        std::unique_ptr<PinLockBox> LockBox = nullptr;
        std::unique_ptr<PinLock> lock                     = nullptr;
        PinLockBox::EnterPasscodeType currentPasscodeType = PinLockBox::EnterPasscodeType::ProvidePasscode;
        gui::Image *lockImage                             = nullptr;
        gui::Image *infoImage                             = nullptr;
        gui::HBox *iceBox                                 = nullptr;

        // method hides or show widgets and sets bars according to provided state
        void buildBottomBar() override;
        void buildTopBar() override;
        void buildTitleBar() override;
        void restore() noexcept override;
        void setVisibleState();
        void buildPinLockBox();
        void invalidate() noexcept override;
        auto usesNumericKeys() const noexcept -> bool;

      public:
        PinLockWindow(app::Application *app, const std::string &window_name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        void buildImages(const std::string &lockImg, const std::string &infoImg);
        [[nodiscard]] auto getToken(Token token) const -> std::string;
        void setImagesVisible(bool lockImg, bool infoImg);
        void setTitleBar(bool isVisible, bool isIceActive);
    };

} /* namespace gui */
