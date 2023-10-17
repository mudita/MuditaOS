// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SummaryListItem.hpp"

#include <gui/widgets/text/TextFixedSize.hpp>
#include <i18n/i18n.hpp>
#include <string>

namespace
{
    std::string format(const std::chrono::minutes minutes)
    {
        using namespace std::chrono;
        const auto hoursPart   = duration_cast<hours>(minutes);
        const auto minutesPart = minutes - hoursPart;

        std::string ret;
        if (hoursPart.count() > 0) {
            ret = std::to_string(hoursPart.count()) + std::string{" h "};
        }
        ret += std::to_string(minutesPart.count()) + std::string{" "} + utils::translate("common_minute_short");

        return ret;
    }

    gui::TextFixedSize *createCenterText(gui::Item *parent)
    {
        using namespace gui;
        auto widget = new TextFixedSize(parent);
        widget->setFont(style::bell_sidelist_item::title_font);
        widget->setEdges(RectangleEdge::None);
        widget->activeItem = false;
        widget->drawUnderline(false);
        return widget;
    }

} // namespace

namespace app::meditation
{
    using namespace gui;
    SummaryListItem::SummaryListItem(const std::string &topDescription,
                                     const std::chrono::minutes total,
                                     const std::chrono::minutes average)
        : BellSideListItemWithCallbacks(topDescription)
    {

        auto duoBox = new VBox(body->getCenterBox());
        duoBox->setEdges(RectangleEdge::None);
        duoBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        duoBox->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);

        auto centerText = createCenterText(duoBox);
        centerText->setMinimumSize(style::bell_base_layout::w, 2 * (style::bell_base_layout::center_layout_h / 3));
        centerText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        centerText->setRichText(utils::translate("app_meditation_summary_total"),
                                text::RichTextParser::TokenMap({{"$VALUE", format(total)}}));

        auto centerText2 = createCenterText(duoBox);
        centerText2->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h / 3);
        centerText2->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        centerText2->setRichText(utils::translate("app_meditation_summary_average"));

        setupBottomTextBox(format(average));
        bottomText->setFont(style::bell_sidelist_item::title_font);
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomText->setEdges(RectangleEdge::None);
    }
} // namespace app::meditation
