// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogItem.hpp"
#include "data/CallLogInternals.hpp"
#include <time/time_conversion_factory.hpp>
#include <gui/widgets/BoxLayout.hpp>

#include <Style.hpp>

namespace gui
{
    CalllogItem::CalllogItem(CalllogModel *model) : model{model}
    {
        setMargins(Margins(0, clItemStyle::top_margin, 0, 0));
        setMinimumSize(clItemStyle::w, clItemStyle::h);
        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);

        auto newImg = [=](const UTF8 &imageName) -> gui::Image * {
            auto img = new gui::Image(hBox, imageName, gui::ImageTypeSpecifier::W_M);
            img->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
            img->setMargins(Margins(0, 0, clItemStyle::internal_margin, 0));
            img->setVisible(false);
            return img;
        };
        imageCallType[calllog::CallLogCallType::IN]     = newImg("calllog_arrow_in");
        imageCallType[calllog::CallLogCallType::OUT]    = newImg("calllog_arrow_out");
        imageCallType[calllog::CallLogCallType::MISSED] = newImg("calllog_arrow_den");

        text = new gui::Label(hBox, 0, 0, 0, 0);
        text->setMinimumHeight(clItemStyle::h);
        text->setMaximumWidth(clItemStyle::w);
        text->setEdges(gui::RectangleEdge::None);
        text->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        text->setFont(style::window::font::big);
        text->setTextEllipsisType(TextEllipsis::Right);

        timestamp = new gui::Label(hBox, 0, 0, 0, 0);
        timestamp->setMargins(Margins(0, 0, clItemStyle::right_margin, 0));
        timestamp->setMinimumHeight(clItemStyle::h);
        timestamp->setMinimumWidth(clItemStyle::timestamp::min_w);
        timestamp->setEdges(gui::RectangleEdge::None);
        timestamp->setFont(style::window::font::small);
        timestamp->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center});

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void CalllogItem::setCall(std::shared_ptr<CalllogRecord> &call)
    {
        this->call = call;
        if (call->presentation == PresentationType::PR_UNKNOWN) {
            text->setText(utils::translate(callLogStyle::strings::privateNumber));
        }
        else {
            text->setText(call->name);
        }

        auto callType = calllog::toCallLogCallType(call->type);
        if (callType == calllog::CallLogCallType::MISSED) {
            text->setFont(style::window::font::bigbold);
        }

        imageCallType[static_cast<uint32_t>(callType)]->setVisible(true);

        using namespace utils::time;
        timestamp->setText(*TimestampFactory().createTimestamp(TimestampType::DateTime, call->date));
    }

} /* namespace gui */
