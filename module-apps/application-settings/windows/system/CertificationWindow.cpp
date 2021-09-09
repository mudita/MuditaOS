// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CertificationWindow.hpp"

#include <application-settings/widgets/SettingsStyle.hpp>
#include <application-settings/windows/WindowNames.hpp>
#include <TextFixedSize.hpp>
#include <ImageBox.hpp>

namespace gui
{
    CertificationWindow::CertificationWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::certification)
    {
        buildInterface();
    }

    void CertificationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_certification"));

        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        auto body = new VBox(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height);
        body->setEdges(RectangleEdge::None);

        createEntryWithTextsAndImage(body,
                                     "United States",
                                     "<text>Contains FCC IDs:<br></br>XMR201903EG25G, RFR-BL871</text>",
                                     "settings_certification_US");

        createEntryWithTextsAndImage(body,
                                     "Canada",
                                     "<text>Contains ISED IDs:<br></br>10224A-201903EG25G, 23249-BL871</text>",
                                     "settings_certification_Canada");

        createEntryWithTextsAndImage(body, "Australia/New Zealand", "", "settings_certification_Australia");

        createEntryWithTitlesAndImages(
            body, "Europe", "settings_certification_Eurlope", "United Kingdom", "settings_certification_UKCA");

        body->resizeItems();
    }

    void CertificationWindow::createEntryWithTextsAndImage(Item *parent,
                                                           const std::string &title,
                                                           const std::string &description,
                                                           const std::string &imageName)
    {
        auto elemHeight = !description.empty() ? style::certification::entry_with_texts_and_image_max_h
                                               : style::certification::entry_with_texts_and_image_min_h;

        auto entryBody = new HBox(parent);
        entryBody->setMinimumSize(style::window::default_body_width, elemHeight);
        entryBody->setEdges(RectangleEdge::None);
        entryBody->setMargins(Margins(0, style::margins::very_big, 0, style::margins::very_big));

        auto textBody = new VBox(entryBody);
        textBody->setMinimumHeight(elemHeight);
        textBody->setMaximumWidth(style::window::default_body_width);
        textBody->setEdges(RectangleEdge::None);

        auto imageBody = new ImageBox(entryBody, 0, 0, 0, 0, new Image(imageName));
        imageBody->setMinimumSizeToFitImage();
        imageBody->setAlignment(Alignment(Alignment::Vertical::Top));

        createTitle(textBody, title);

        if (!description.empty()) {
            auto descriptionText = new TextFixedSize(textBody, 0, 0, 0, 0);
            descriptionText->drawUnderline(false);
            descriptionText->setFont(style::window::font::small);
            descriptionText->setMinimumHeightToFitText(2);
            descriptionText->setMaximumWidth(style::window::default_body_width);
            descriptionText->setMargins(Margins(0, style::margins::very_big, 0, 0));
            descriptionText->setRichText(description);
        }
    }

    void CertificationWindow::createTitle(Item *parent, const std::string &title)
    {
        auto titleText = new TextFixedSize(parent, 0, 0, 0, 0);
        titleText->drawUnderline(false);
        titleText->setFont(style::window::font::smallbold);
        titleText->setMinimumHeightToFitText();
        titleText->setMaximumWidth(style::window::default_body_width);
        titleText->setMaximumHeight(style::certification::entry_with_texts_and_image_max_h);
        titleText->setAlignment(Alignment(Alignment::Vertical::Center));
        titleText->setText(title);
    }

    void CertificationWindow::createTitleAndImage(Item *parent, const std::string &title, const std::string &imageName)
    {
        auto entryBody = new VBox(parent);
        entryBody->setMinimumHeight(style::certification::entry_with_texts_and_image_max_h);
        entryBody->setMaximumWidth(style::window::default_body_width / 2);
        entryBody->setEdges(RectangleEdge::None);

        createTitle(entryBody, title);

        auto imageBody = new ImageBox(entryBody, 0, 0, 0, 0, new Image(imageName));
        imageBody->setMinimumSizeToFitImage();
        imageBody->setMinimumHeight(style::certification::entry_with_texts_and_image_min_h);
        imageBody->setAlignment(Alignment(Alignment::Vertical::Bottom));
    }

    void CertificationWindow::createEntryWithTitlesAndImages(Item *parent,
                                                             const std::string &title1,
                                                             const std::string &imageName1,
                                                             const std::string &title2,
                                                             const std::string &imageName2)
    {
        auto entryBody = new HBox(parent);
        entryBody->setMinimumSize(style::window::default_body_width,
                                  style::certification::entry_with_texts_and_image_max_h);
        entryBody->setEdges(RectangleEdge::None);
        entryBody->setMargins(Margins(0, style::margins::very_big, 0, style::margins::very_big));

        createTitleAndImage(entryBody, title1, imageName1);
        createTitleAndImage(entryBody, title2, imageName2);
    }
} // namespace gui
