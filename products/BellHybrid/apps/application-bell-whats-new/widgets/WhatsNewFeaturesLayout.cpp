// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewFeaturesLayout.hpp"
#include "WhatsNewStyle.hpp"
#include <TextFixedSize.hpp>

namespace gui
{
    WhatsNewFeaturesLayout::WhatsNewFeaturesLayout(const std::string &title,
                                                   const std::string &description,
                                                   const std::string &iconName,
                                                   bool leftArrowState,
                                                   bool rightArrowState)
        : VBox(nullptr,
               0,
               0,
               whats_new_style::features_window::layout::width,
               whats_new_style::features_window::layout::height)
    {
        buildInterface(title, description, iconName, leftArrowState, rightArrowState);
    }

    auto WhatsNewFeaturesLayout::buildInterface(const std::string &title,
                                                const std::string &description,
                                                const std::string &iconName,
                                                bool leftArrowState,
                                                bool rightArrowState) -> void
    {
        setAlignment(Alignment::Horizontal::Center);
        setEdges(rectangle_enums::RectangleEdge::None);

        /* Main container */
        auto mainContainer = new HThreeBox<HBox, VBox, HBox>(this);
        mainContainer->setMinimumSize(whats_new_style::features_window::container::width,
                                      whats_new_style::features_window::container::height);
        mainContainer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        mainContainer->setMargins(Margins(0, whats_new_style::features_window::container::margin_top, 0, 0));
        mainContainer->setEdges(RectangleEdge::None);

        /* Left box - with arrow */
        mainContainer->firstBox = new HBox(mainContainer);
        mainContainer->firstBox->setAlignment(Alignment(Alignment::Vertical::Center));
        mainContainer->firstBox->setEdges(RectangleEdge::None);
        mainContainer->firstBox->activeItem = false;

        auto leftArrow = new ImageBox(nullptr, new Image("bell_arrow_left_W_M"));
        leftArrow->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        leftArrow->setMinimumSizeToFitImage();
        leftArrow->setVisible(leftArrowState);
        leftArrow->setEdges(RectangleEdge::None);
        mainContainer->firstBox->setMinimumSize(leftArrow->widgetMinimumArea.w, leftArrow->widgetMinimumArea.h);
        mainContainer->firstBox->addWidget(leftArrow);

        /* Center box - icon, title and description */
        mainContainer->centerBox = new VBox(mainContainer);
        mainContainer->centerBox->setEdges(RectangleEdge::None);
        mainContainer->centerBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        mainContainer->centerBox->setMinimumSize(whats_new_style::features_window::center_box::width,
                                                 whats_new_style::features_window::center_box::height);
        mainContainer->centerBox->setMaximumSize(whats_new_style::features_window::center_box::width,
                                                 whats_new_style::features_window::center_box::height);

        auto iconImage = new ImageBox(nullptr, new Image(iconName));
        iconImage->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        iconImage->setMinimumSizeToFitImage();
        iconImage->setMaximumSize(whats_new_style::features_window::icon::width,
                                  whats_new_style::features_window::icon::height);

        auto titleText = new Text(nullptr,
                                  0,
                                  0,
                                  whats_new_style::features_window::title::width,
                                  whats_new_style::features_window::title::height);
        titleText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        titleText->setMargins(Margins(0, whats_new_style::features_window::title::margin_top, 0, 0));
        titleText->setTextType(TextType::SingleLine);
        titleText->setFont(whats_new_style::features_window::title::font);
        titleText->setText(title);

        auto descriptionText = new Text(nullptr,
                                        0,
                                        0,
                                        whats_new_style::features_window::description::width,
                                        whats_new_style::features_window::description::height);
        descriptionText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        descriptionText->setMargins(Margins(0, whats_new_style::features_window::description::margin_top, 0, 0));
        descriptionText->setFont(whats_new_style::features_window::description::font);
        descriptionText->setText(description);

        mainContainer->centerBox->addWidget(iconImage);
        mainContainer->centerBox->addWidget(titleText);
        mainContainer->centerBox->addWidget(descriptionText);

        /* Right box */
        mainContainer->lastBox = new HBox(mainContainer);
        mainContainer->lastBox->setAlignment(Alignment(Alignment::Vertical::Center));
        mainContainer->lastBox->setEdges(RectangleEdge::None);
        mainContainer->lastBox->activeItem = false;

        auto rightArrow = new ImageBox(nullptr, new Image("bell_arrow_right_W_M"));
        rightArrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        rightArrow->setMinimumSizeToFitImage();
        rightArrow->setVisible(rightArrowState);
        rightArrow->setEdges(RectangleEdge::None);
        mainContainer->lastBox->setMinimumSize(rightArrow->widgetMinimumArea.w, rightArrow->widgetMinimumArea.h);
        mainContainer->lastBox->addWidget(rightArrow);

        resizeItems();
    }
} // namespace gui
