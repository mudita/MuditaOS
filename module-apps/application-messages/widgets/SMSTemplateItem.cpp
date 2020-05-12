#include "SMSTemplateItem.hpp"

#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui
{
    namespace smsTemplItemStyle
    {
        constexpr uint32_t w = style::window::default_body_width;
        constexpr uint32_t h = style::window::label::big_h;
    } // namespace smsTemplItemStyle

    SMSTemplateItem::SMSTemplateItem()
    {
        minWidth  = smsTemplItemStyle::w;
        minHeight = smsTemplItemStyle::h;
        maxWidth  = smsTemplItemStyle::w;
        maxHeight = smsTemplItemStyle::h;

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
