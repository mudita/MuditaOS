// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "Text.hpp"
#include "BoxLayout.hpp"

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
        void buildPinLabels(std::function<Rect *()> itemBuilder,
                            unsigned int pinSize,
                            unsigned int offsetX,
                            unsigned int offsetY,
                            unsigned int boxWidth);
        void buildImages(const std::string &lockImg, const std::string &infoImg);
        void setBottomBarWidgetsActive(bool left, bool center, bool right);
        void setImagesVisible(bool lockImg, bool infoImg);
        void setBottomBarWidgetText(BottomBar::Side side, const UTF8 &txt);

        gui::Label *titleLabel = nullptr;
        gui::Text *infoText    = nullptr;
        gui::Image *lockImage = nullptr;
        gui::Image *infoImage = nullptr;
        gui::HBox *pinLabelsBox = nullptr;

        PinLock &lock;

      private:
        void buildBottomBar();
        void buildTopBar();
        void buildTitleLabel();
    };
} // namespace gui
