// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include <AppWindow.hpp>
#include <BoxLayout.hpp>
#include <Image.hpp>
#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{

    // class DesktopInputWidget : public ListItem
    class DesktopInputWidget : public HBox
    {
        app::ApplicationCommon *application = nullptr;
        HBox *body                    = nullptr;
        gui::Image *replyImage        = nullptr;

      public:
        gui::Text *inputText = nullptr;

        DesktopInputWidget(
            app::ApplicationCommon *application, Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        ~DesktopInputWidget() override = default;

        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

} /* namespace gui */
