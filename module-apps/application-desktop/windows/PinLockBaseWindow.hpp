// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "Text.hpp"

namespace gui
{
    class PinLock;
}

namespace gui
{
    class PinLockBaseWindow : public AppWindow
    {
      public:
        PinLockBaseWindow(app::Application *app, std::string name, PinLock &lock) : AppWindow(app, name), lock(lock)
        {}
        void build();
        void buildInfoText(unsigned int textHight);
        void buildPinLabels(gui::Label *labelBox, unsigned int pinSize, unsigned int singleLabelWidth);
        void buildImages(const std::string &lockImg, const std::string &infoImg);
        void setBottomBarWidgetsActive(bool left, bool center, bool right);
        void setImagesVisible(bool lockImg, bool infoImg);
        void setBottomBarWidgetText(BottomBar::Side side, const UTF8 &txt);
        void clearPinLabels();

        gui::Label *titleLabel = nullptr;
        gui::Text *infoText    = nullptr;
        gui::Label *pinLabel   = nullptr;
        std::vector<gui::Label *> pinLabels;
        gui::Image *lockImage = nullptr;
        gui::Image *infoImage = nullptr;

        PinLock &lock;

      private:
        void buildBottomBar();
        void buildTopBar();
        void buildTitleLabel();
    };
} // namespace gui
