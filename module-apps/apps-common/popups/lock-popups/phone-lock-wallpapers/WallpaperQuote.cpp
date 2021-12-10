// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperQuote.hpp"
#include <service-appmgr/Controller.hpp>
#include <ImageBox.hpp>
#include <Text.hpp>
#include <Style.hpp>

namespace gui
{
    WallpaperQuote::WallpaperQuote(app::ApplicationCommon *app, Item *parent)
        : WallpaperBase(parent), quotesPresenter{std::make_unique<QuotesPresenter>(app)}
    {
        quotesPresenter->setQuoteDisplayCallback([&](std::string quote, std::string author) {
            quoteText->setText(quote);
            quoteText->setMinimumWidthToFitText();
            quoteText->setMinimumHeightToFitText();
            authorText->setText(std::string("- ") + author);
            authorText->setMinimumWidthToFitText();
            authorText->setMinimumHeightToFitText();
            textBox->resizeItems();
        });
    }

    void WallpaperQuote::build()
    {
        textBox = new VBox(parent,
                           ::style::window::default_left_margin,
                           style::textBox::y,
                           ::style::window::default_body_width,
                           style::textBox::h);
        textBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        textBox->setEdges(RectangleEdge::None);

        auto quoteImage = new ImageBox(textBox, new Image("quote", ImageTypeSpecifier::W_G));
        quoteImage->setMinimumSizeToFitImage();
        quoteImage->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        quoteImage->setMargins(Margins(0, 0, 0, 0));

        quoteText = new TextFixedSize(textBox, 0, 0, 0, 0);
        quoteText->setMaximumSize(::style::window::default_body_width, style::text::h);
        quoteText->setMargins(Margins(0, style::text::topMarigin, 0, style::text::bottomMarigin));
        quoteText->setFont(::style::window::font::biglight);
        quoteText->setLinesSpacing(style::text::interline);
        quoteText->setEdges(RectangleEdge::None);
        quoteText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        quoteText->drawUnderline(false);

        authorText = new Text(textBox, 0, 0, 0, 0);
        authorText->setMargins(Margins(0, 0, 0, 0));
        authorText->setFont(::style::window::font::small);
        authorText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        hide();
    }

    void WallpaperQuote::show()
    {
        quotesPresenter->requestQuote();
        textBox->setVisible(true);
        textBox->resizeItems();
    }

    void WallpaperQuote::hide()
    {
        textBox->setVisible(false);
        textBox->resizeItems();
    }

} /* namespace gui */
