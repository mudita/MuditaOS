#include "SMSTemplateItem.hpp"

#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui
{

    namespace smsTemplItemStyle
    {
        constexpr uint32_t w = 440;
        constexpr uint32_t h = style::window::label::big_h;
    } // namespace smsTemplItemStyle

    SMSTemplateItem::SMSTemplateItem(SMSTemplateModel *model) : model{model}
    {
        minWidth  = smsTemplItemStyle::w;
        minHeight = smsTemplItemStyle::h;
        maxWidth  = smsTemplItemStyle::w;
        maxHeight = smsTemplItemStyle::h;

        setRadius(0);
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

        setPenFocusWidth(style::window::default_border_focucs_w);
        setPenWidth(style::window::default_border_no_focus_w);

        text = new gui::Label(this, 0, 0, 0, 0);
        style::window::decorate(text);
        text->setFont(style::window::font::big);
        text->setDotsMode(true);
    }

    bool SMSTemplateItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        text->setPosition(0, 0);
        text->setSize(smsTemplItemStyle::w, newDim.h);

        return true;
    }

    void SMSTemplateItem::setTemplate(std::shared_ptr<SMSTemplateRecord> &templ)
    {
        this->templ = templ;
        text->setText(templ->text);
    }

} /* namespace gui */
