/*
 * @file CalllogItem.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log Item
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "CalllogItem.hpp"
#include "time/time_conversion.hpp"
#include "../data/CallLogInternals.hpp"

#include <Style.hpp>

using namespace calllog;

namespace gui
{

    CalllogItem::CalllogItem(CalllogModel *model, bool mode24H) : model{model}, mode24H{mode24H}
    {
        setMinimumSize(clItemStyle::w, clItemStyle::h);
        setMaximumSize(clItemStyle::w, clItemStyle::h);

        setRadius(0);
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

        setPenFocusWidth(style::window::default_border_focucs_w);
        setPenWidth(style::window::default_border_no_focus_w);

        timestamp = new gui::Label(this, 0, 0, 0, 0);
        style::window::decorate(timestamp);
        timestamp->setFont(style::window::font::big);
        timestamp->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_CENTER});

        auto newImg = [=](const UTF8 imageName) -> gui::Image * {
            auto img = new gui::Image(this, clItemStyle::img::x, clItemStyle::img::y, 0, 0, imageName);
            img->setVisible(false);
            return img;
        };
        imageCallType[calllog::CallLogCallType::IN]     = newImg("calllog_arrow_in");
        imageCallType[calllog::CallLogCallType::OUT]    = newImg("calllog_arrow_out");
        imageCallType[calllog::CallLogCallType::MISSED] = newImg("calllog_arrow_den");

        text = new gui::Label(this, 0, 0, 0, 0);
        style::window::decorate(text);
        text->setFont(style::window::font::big);
        text->setEllipsis(Ellipsis::Right);
    }

    bool CalllogItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        text->setPosition(clItemStyle::text::x, 0);
        text->setSize(newDim.w, newDim.h);

        timestamp->setPosition(newDim.w - clItemStyle::timestamp::w, 0);
        timestamp->setSize(clItemStyle::timestamp::w, newDim.h);
        return true;
    }

    void CalllogItem::setCall(std::shared_ptr<CalllogRecord> &call)
    {
        this->call = call;
        text->setText(call->name);

        auto callType = calllog::toCallLogCallType(call->type);
        if (callType == calllog::CallLogCallType::MISSED)
            text->setFont(style::window::font::bigbold);

        imageCallType[static_cast<uint32_t>(callType)]->setVisible(true);

        timestamp->setText(utils::time::DateTime(call->date, false)); // TODO: alek: check for AM/PM and 24h
    }

} /* namespace gui */
