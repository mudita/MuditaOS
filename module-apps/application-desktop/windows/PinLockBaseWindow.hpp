// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LockWindow.hpp"

namespace gui
{
    class PinLockBaseWindow : public LockWindow
    {
      public:
        PinLockBaseWindow(app::Application *app, std::string name) : LockWindow(app, name)
        {}
        void buildImages(const std::string &lockImg, const std::string &infoImg);
        [[nodiscard]] auto getToken(Token token) const -> std::string;
        void restore() noexcept override;
        void setImagesVisible(bool lockImg, bool infoImg);
        void setTitleBar(bool isVisible, bool isIceActive);

        gui::HBox *iceBox        = nullptr;
        gui::Image *infoImage = nullptr;
        gui::Image *lockImage    = nullptr;

      private:
        void buildBottomBar() override;
        void buildTitleBar() override;
        void buildTopBar() override;
    };
} // namespace gui
