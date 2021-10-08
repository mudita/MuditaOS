// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <Rect.hpp>

namespace gui
{

    class ContactFlagIconWidget : public Rect
    {
      public:
        ContactFlagIconWidget(const std::string &iconName, const std::string &itemText, Item *parent = nullptr);
        ContactFlagIconWidget(uint8_t iconNumber, const std::string &itemText, Item *parent = nullptr);
        void setIconNumber(uint8_t iconNumber);

      private:
        void buildItemWithIcon();
        void buildItemWithText();
        void buildItem();
        std::string iconName;
        uint8_t number = 0;
        std::string itemText;
        bool numericIcon    = false;
        Image *icon         = nullptr;
        Label *numericLabel = nullptr;
        Label *label        = nullptr;
        VBox *mainBox       = nullptr;
    };

} // namespace gui
