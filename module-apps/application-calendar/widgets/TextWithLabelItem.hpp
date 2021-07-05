// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class TextWithLabelItem : public CalendarListItem
    {
        gui::VBox *vBox              = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Text *textInput         = nullptr;

      public:
        TextWithLabelItem(const std::string &description,
                          std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                          std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                          std::function<void()> selectSpecialCharacter                 = nullptr);
        virtual ~TextWithLabelItem() override = default;
    };

} /* namespace gui */
