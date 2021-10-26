// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmInternalListItem.hpp"
#include <Text.hpp>

namespace gui
{
    class AlarmTimeItem : public AlarmInternalListItem
    {
        gui::HBox *hBox          = nullptr;
        gui::Label *colonLabel   = nullptr;
        gui::Text *hourInput     = nullptr;
        gui::Text *minuteInput   = nullptr;
        gui::Label *mode12hInput = nullptr;
        bool mode24H             = false;

        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;

        void applyItemSpecificProperties(gui::Text *item);
        void applyInputCallbacks();
        void onInputCallback(gui::Text &textItem);
        void prepareForTimeMode();
        [[nodiscard]] bool isPm(const std::string &text) const;
        void validateHour();

      public:
        AlarmTimeItem(std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                      std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);
    };

} /* namespace gui */
