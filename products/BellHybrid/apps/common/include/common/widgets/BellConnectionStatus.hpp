// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/BoxLayout.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class BellConnectionStatus : public gui::HBox
    {
      public:
        explicit BellConnectionStatus(Item *parent);
        void setFont(const UTF8 &fontName);
        void show(bool visibility);
        bool isVisible() const;

      private:
        Text *statusText = nullptr;
    };
} // namespace gui
