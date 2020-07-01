#include "BaseThreadItem.hpp"
#include "../MessagesStyle.hpp"

namespace gui
{

    BaseThreadItem::BaseThreadItem()
    {
        using namespace style;
        setMargins(Margins(0, style::margins::small, 0, style::margins::small));
        setMinimumSize(window::default_body_width, style::window::messages::sms_thread_item_h);
        setMaximumSize(window::default_body_width, style::window::messages::sms_thread_item_h);

        setRadius(0);
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

        setPenFocusWidth(window::default_border_focus_w);
        setPenWidth(window::default_border_no_focus_w);

        contact = new gui::Label(this, 0, 0, 0, 0);
        contact->setPenFocusWidth(window::default_border_no_focus_w);
        contact->setPenWidth(window::default_border_no_focus_w);
        contact->setFont(style::window::font::big);
        contact->setEllipsis(Ellipsis::Right);
        contact->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER});

        timestamp = new gui::Label(this, 0, 0, 0, 0);
        timestamp->setPenFocusWidth(window::default_border_no_focus_w);
        timestamp->setPenWidth(window::default_border_no_focus_w);
        timestamp->setFont(style::window::font::small);
        timestamp->setEllipsis(Ellipsis::Right);
        timestamp->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_CENTER});

        preview = new gui::Label(this, 0, 0, 0, 0);
        preview->setPenFocusWidth(window::default_border_no_focus_w);
        preview->setPenWidth(window::default_border_no_focus_w);
        preview->setFont(style::window::font::small);
        preview->setEllipsis(Ellipsis::Right);
        preview->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER});
    }

    void BaseThreadItem::onDimensionChangedTop(const BoundingBox & /*oldDim*/, const BoundingBox &newDim)
    {
        namespace msgStyle = style::messages::threadItem;

        contact->setPosition(msgStyle::leftMargin, msgStyle::topMargin);
        contact->setSize(newDim.w - msgStyle::cotactWidthOffset, newDim.h / 2 - msgStyle::topMargin);

        timestamp->setPosition(newDim.w - msgStyle::timestampWidth, msgStyle::topMargin);
        timestamp->setSize(msgStyle::timestampWidth, newDim.h / 2 - msgStyle::topMargin);
    }

    void BaseThreadItem::onDimensionChangedBottom(const BoundingBox & /*oldDim*/, const BoundingBox &newDim)
    {
        namespace msgStyle = style::messages::threadItem;

        preview->setPosition(msgStyle::leftMargin, newDim.h / 2);
        preview->setSize(newDim.w - msgStyle::previewWidthOffset, newDim.h / 2 - msgStyle::bottomMargin);
    }

    bool BaseThreadItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        onDimensionChangedTop(oldDim, newDim);
        onDimensionChangedBottom(oldDim, newDim);

        return true;
    }
} // namespace gui
