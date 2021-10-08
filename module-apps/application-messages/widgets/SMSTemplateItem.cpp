// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSTemplateItem.hpp"

#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui
{
    SMSTemplateItem::SMSTemplateItem()
    {
        setMargins(Margins(0, style::margins::big, 0, 0));
        setMinimumSize(style::smsTemplItem::w, style::smsTemplItem::h);
        setMaximumSize(style::smsTemplItem::w, style::smsTemplItem::h);

        text = new gui::Label(this, 0, 0, 0, 0);
        style::window::decorateOption(text);
        text->setEllipsis(gui::Ellipsis::Right);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            text->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void SMSTemplateItem::setTemplate(std::shared_ptr<SMSTemplateRecord> templ)
    {
        this->templ = templ;
        text->setText(templ->text);
    }

} /* namespace gui */
