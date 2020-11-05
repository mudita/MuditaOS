// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogItem.hpp"
#include "time/time_conversion.hpp"
#include "../data/CallLogInternals.hpp"
#include "gui/widgets/BoxLayout.hpp"

#include <Style.hpp>

using namespace calllog;

namespace gui
{

    CalllogItem::CalllogItem(CalllogModel *model, bool mode24H) : model{model}, mode24H{mode24H}
    {
        setMargins(Margins(0, style::margins::big, 0, 0));
        setMinimumSize(clItemStyle::w, clItemStyle::h);
        setMaximumSize(clItemStyle::w, clItemStyle::h);

        setRadius(0);
        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);

        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        timestamp = new gui::Label(this, 0, 0, 0, 0);
        style::window::decorate(timestamp);
        timestamp->setFont(style::window::font::big);
        timestamp->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center});

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

        timestamp->setText(utils::time::DateTime(utils::time::TimeConversion::toTimePoint(call->date), false)); // TODO: alek: check for AM/PM and 24h
    }

} /* namespace gui */
