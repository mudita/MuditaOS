// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperQuote.hpp"
#include <service-appmgr/Controller.hpp>
#include <ImageBox.hpp>
#include <Text.hpp>
#include <Style.hpp>
#include <notifications/NotificationTilesBox.hpp>
namespace gui
{
    WallpaperQuote::WallpaperQuote(app::ApplicationCommon *app, Item *parent)
        : WallpaperBase(parent), quotesPresenter{std::make_unique<QuotesPresenter>(app)},
          notificationsPresenter(std::make_shared<NotificationTilesPresenter>())
    {
        quotesPresenter->setQuoteDisplayCallback([&](std::string quote, std::string author) {
            quoteText->setText(quote);
            quoteText->setMinimumWidthToFitText();
            quoteText->setMinimumHeightToFitText();
            authorText->setText(std::string("- ") + author);
            authorText->setMinimumWidthToFitText();
            authorText->setMinimumHeightToFitText();
            wallpaperBox->resizeItems();
        });

        wallpaperBox = new VBox(parent,
                                ::style::window::default_left_margin,
                                ::style::wallpaper::wallpaperBox::y,
                                ::style::window::default_body_width,
                                ::style::wallpaper::wallpaperBox::h);
        wallpaperBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        wallpaperBox->setEdges(RectangleEdge::None);

        auto quoteImage = new ImageBox(wallpaperBox, new Image("quote", ImageTypeSpecifier::W_G));
        quoteImage->setMinimumSizeToFitImage();
        quoteImage->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        quoteImage->setMargins(Margins(0, 0, 0, 0));

        quoteText = new TextFixedSize(wallpaperBox, 0, 0, 0, 0);
        quoteText->setMaximumSize(::style::window::default_body_width, style::text::h);
        quoteText->setMargins(Margins(0, style::text::topMarigin, 0, style::text::bottomMarigin));
        quoteText->setFont(::style::window::font::biglight);
        quoteText->setLinesSpacing(style::text::interline);
        quoteText->setEdges(RectangleEdge::None);
        quoteText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        quoteText->drawUnderline(false);
        quoteText->setTextLimitType(TextLimitType::MaxLines, style::text::maxLineCount);

        authorText = new Text(wallpaperBox, 0, 0, 0, 0);
        authorText->setMargins(Margins(0, 0, 0, style::text::authorBottomMarigin));
        authorText->setFont(::style::window::font::small);
        authorText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        new NotificationTilesBox(wallpaperBox, notificationsPresenter);

        hide();
    }

    void WallpaperQuote::show()
    {
        quotesPresenter->requestQuote();
        wallpaperBox->setVisible(true);
        wallpaperBox->resizeItems();
    }

    void WallpaperQuote::hide()
    {
        wallpaperBox->setVisible(false);
        wallpaperBox->resizeItems();
    }

    std::shared_ptr<NotificationsPresenter> WallpaperQuote::getNotificationsPresenter()
    {
        return notificationsPresenter;
    }
} /* namespace gui */
