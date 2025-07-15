// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "layouts/HomeScreenLayoutClassicWithQuotes.hpp"
#include "data/BellMainStyle.hpp"

#include <widgets/text/TextFixedSize.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>
#include <widgets/BellConnectionStatus.hpp>
#include <widgets/BellBattery.hpp>

namespace
{
    constexpr auto maxTimeBoxHeight{148U};
    constexpr auto textBoxHeight{120U};
    constexpr auto infoBoxHeight{54U};

    constexpr auto imgBoxHeight{52U};
    constexpr auto imgTopMargin{10U};

    constexpr auto quoteImageName{"bell_quote"};
    constexpr auto quoteFont{style::window::font::mediumbiglight};
    constexpr auto quoteHeight{70U};

    constexpr auto authorFont{style::window::font::mediumbigbold};
    constexpr auto authorHeight{42U};
} // namespace

namespace gui
{
    HomeScreenLayoutClassicWithQuotes::HomeScreenLayoutClassicWithQuotes(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
        onShowMessage = [this]() {
            hideQuotes();
            adjustWidgetBox();
        };
        onHideMessage = [this]() {
            if (!connectionStatus->isVisible() && !battery->visible) {
                showQuotes();
            }
        };
    }

    void HomeScreenLayoutClassicWithQuotes::buildInterface()
    {
        using namespace bellMainStyle;

        this->centerBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        timeHBox->setMaximumHeight(maxTimeBoxHeight);
        timeHBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        time->setMaximumHeight(maxTimeBoxHeight);
        time->setFont(style::window::font::huge);
        time->setMargins({0, 0, 0, 0});

        imgBox = new VBox(this->centerBox);
        imgBox->setMinimumSize(style::homescreen_classic::status_box_layout_w, imgBoxHeight);
        imgBox->setEdges(RectangleEdge::None);
        imgBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        imgBox->setVisible(true);

        quoteImg = new Image(imgBox, quoteImageName, gui::ImageTypeSpecifier::W_M);
        quoteImg->setMargins({0, imgTopMargin, 0, 0});
        quoteImg->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        widgetBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        infoBox->setMinimumHeight(infoBoxHeight);
        infoBox->setMaximumHeight(infoBoxHeight);
        infoBox->setMargins(Margins(0, 0, 0, 0));

        connectionBox->setMargins(Margins(0, 0, 0, 0));
        battery->setMargins(Margins(0, 0, 0, 0));

        textBox = new VBox(nullptr);
        textBox->setMinimumSize(style::bell_base_layout::last_layout_w, textBoxHeight);
        textBox->setEdges(RectangleEdge::None);
        textBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        textBox->setVisible(true);

        quotes = new TextFixedSize(nullptr);
        quotes->setMaximumSize(style::bell_base_layout::last_layout_w, quoteHeight);
        quotes->setMargins({0, 0, 0, 0});
        quotes->setFont(quoteFont);
        quotes->setText("");
        quotes->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        quotes->setEdges(RectangleEdge::None);
        quotes->activeItem = false;
        quotes->drawUnderline(false);
        quotes->setVisible(true);

        author = new TextFixedSize(nullptr);
        author->setMaximumSize(style::bell_base_layout::last_layout_w, authorHeight);
        author->setMargins({0, 0, 0, 0});
        author->setFont(authorFont);
        author->setText("");
        author->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        author->setEdges(RectangleEdge::None);
        author->activeItem = false;
        author->drawUnderline(false);
        author->setVisible(true);

        textBox->addWidget(quotes);
        textBox->addWidget(author);

        statusBox->setVisible(false);
        this->lastBox->addWidget(textBox);

        resizeItems();
    }

    void HomeScreenLayoutClassicWithQuotes::showQuotes()
    {
        if (!textBox->visible) {
            statusBox->setVisible(false);
            textBox->setVisible(true);
            textBox->informContentChanged();
            imgBox->setVisible(true);
            imgBox->informContentChanged();
        }
    }

    void HomeScreenLayoutClassicWithQuotes::hideQuotes()
    {
        if (textBox->visible) {
            textBox->setVisible(false);
            imgBox->setVisible(false);
            statusBox->setVisible(true);
            statusBox->informContentChanged();
        }
    }

    void HomeScreenLayoutClassicWithQuotes::setQuoteText(const UTF8 &quoteContent, const UTF8 &quoteAuthor)
    {
        constexpr auto dash{'-'};
        UTF8 authorWithDash = quoteAuthor;
        if (!quoteAuthor.empty()) {
            authorWithDash.insert(&dash, 0);
        }
        author->setText(authorWithDash);
        quotes->setText(quoteContent);
    }

    void HomeScreenLayoutClassicWithQuotes::adjustWidgetBox()
    {
        infoBox->setVisible(battery->visible);
        connectionBox->setVisible(connectionStatus->isVisible());
        widgetBox->informContentChanged();
    }
} // namespace gui
