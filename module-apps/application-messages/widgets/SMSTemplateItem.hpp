// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/SMSTemplateRecord.hpp>
#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>

namespace style::smsTemplItem
{
    constexpr uint32_t w = style::window::default_body_width;
    constexpr uint32_t h = style::window::label::big_h;
} // namespace style::smsTemplItem

namespace gui
{
    class SMSTemplateItem : public ListItem
    {
        std::shared_ptr<SMSTemplateRecord> templ = nullptr;
        gui::Label *text                         = nullptr;

      public:
        SMSTemplateItem();
        virtual ~SMSTemplateItem() = default;
        void setTemplate(std::shared_ptr<SMSTemplateRecord>);
    };

} /* namespace gui */
