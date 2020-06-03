#include "SMSTemplateItem.hpp"

#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui
{
    SMSTemplateItem::SMSTemplateItem()
    {
        setMinimumSize(style::smsTemplItem::w, style::smsTemplItem::h);
        setMaximumSize(style::smsTemplItem::w, style::smsTemplItem::h);

        text = new gui::Label(this, 0, 0, 0, 0);
        style::window::decorateOption(text);
        text->setEllipsis(gui::Ellipsis::Right);
    }

    bool SMSTemplateItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        text->setPosition(0, 0);
        text->setSize(newDim.w, newDim.h);

        return true;
    }

    void SMSTemplateItem::setTemplate(std::shared_ptr<SMSTemplateRecord> templ)
    {
        this->templ = templ;
        text->setText(templ->text);
    }

} /* namespace gui */
