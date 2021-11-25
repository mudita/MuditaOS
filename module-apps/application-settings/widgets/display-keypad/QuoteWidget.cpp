// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuoteWidget.hpp"

#include <i18n/i18n.hpp>

namespace style::quotes
{
    namespace widget
    {
        inline constexpr uint32_t w = style::window::default_body_width;
        inline constexpr uint32_t h = 50;

        inline constexpr uint32_t input_box_w           = 55;
        inline constexpr uint32_t input_box_h           = h;
        inline constexpr int32_t input_box_right_margin = 20;

        inline constexpr uint32_t description_label_w           = style::window::default_body_width;
        inline constexpr uint32_t description_label_h           = 33;
        inline constexpr int32_t description_label_right_margin = 40;

        inline constexpr int32_t tick_image_left_margin  = -64;
        inline constexpr int32_t tick_image_right_margin = 32;

    } // namespace widget

} // namespace style::quotes

namespace gui
{

    QuoteWidget::QuoteWidget(const Quotes::QuoteRecord &quoteRecord,
                             std::function<void(bool)> enableQuoteCallback,
                             std::function<void(const UTF8 &)> navBarTemporaryMode,
                             std::function<void()> navBarRestoreFromTemporaryMode)
        : quote(quoteRecord), enableQuote(std::move(enableQuoteCallback)),
          navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode))
    {

        setMinimumSize(style::quotes::widget::w, style::quotes::widget::h);

        setMargins(gui::Margins(0, style::margins::big, 0, 0));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);

        tickImage = new ImageBox(
            hBox, 0, 0, style::widgets::iconsSize, style::widgets::iconsSize, new Image("small_tick_32px_W_M"));
        tickImage->setVisible(true);
        tickImage->showImage(quote.enabled);
        tickImage->setEdges(RectangleEdge::Bottom);

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setMaximumSize(style::quotes::widget::description_label_w,
                                         style::quotes::widget::description_label_h);
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        descriptionLabel->setText(quote.quote);

        focusChangedCallback = [&](gui::Item &item) {
            if (item.focus) {
                descriptionLabel->setFont(style::window::font::mediumbold);
                auto bottorBarText =
                    quote.enabled ? utils::translate("common_uncheck") : utils::translate("common_check");
                this->navBarTemporaryMode(bottorBarText);
            }
            else {
                descriptionLabel->setFont(style::window::font::medium);
                setFocusItem(nullptr);
                this->navBarRestoreFromTemporaryMode();
            }
            return true;
        };

        activatedCallback = [&](gui::Item &item) {
            quote.enabled = !quote.enabled;
            enableQuote(quote.enabled);
            tickImage->showImage(quote.enabled);
            auto bottorBarText = quote.enabled ? utils::translate("common_uncheck") : utils::translate("common_check");
            this->navBarTemporaryMode(bottorBarText);
            hBox->resizeItems();
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        setEdges(gui::RectangleEdge::None);
    }
} /* namespace gui */
