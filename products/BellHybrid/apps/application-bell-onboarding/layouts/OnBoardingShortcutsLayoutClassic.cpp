// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingShortcutsLayoutClassic.hpp"
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <i18n/i18n.hpp>
#include <Style.hpp>
#include <ImageBox.hpp>
#include <TextFixedSize.hpp>

namespace
{
    namespace container
    {
        constexpr inline auto width      = 544U;
        constexpr inline auto height     = 220U;
        constexpr inline auto top_margin = 74U;

        namespace separator
        {
            constexpr inline auto height = 22U;
        } // namespace separator

        namespace image
        {
            constexpr inline auto height = container::height - separator::height;
            constexpr inline auto width  = 448U;
        } // namespace image

        namespace text
        {
            constexpr inline auto height = 146U;
            constexpr inline auto width  = 448U;
        } // namespace text

    } // namespace container
} // namespace

namespace gui
{
    OnBoardingShortcutsLayoutClassic::OnBoardingShortcutsLayoutClassic(const UTF8 &image,
                                                                       const UTF8 &description,
                                                                       bool leftArrowVisible,
                                                                       bool rightArrowVisible)
        : OnBoardingShortcutsLayoutProvider(image, description),
          VBox(nullptr, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h), image{image},
          description{description}, leftArrowVisible{leftArrowVisible}, rightArrowVisible{rightArrowVisible}
    {
        buildInterface();
    }

    void OnBoardingShortcutsLayoutClassic::buildInterface()
    {
        setAlignment(Alignment::Horizontal::Center);

        auto containerThreeBox = new HThreeBox<HBox, HBox, HBox>(this);
        containerThreeBox->setMinimumSize(container::width, container::image::height);
        containerThreeBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        containerThreeBox->setMargins(Margins(0, container::top_margin, 0, 0));
        containerThreeBox->setEdges(RectangleEdge::None);

        containerThreeBox->firstBox = new HBox(containerThreeBox);
        containerThreeBox->firstBox->setAlignment(Alignment(Alignment::Vertical::Top));
        containerThreeBox->firstBox->setEdges(RectangleEdge::None);
        containerThreeBox->firstBox->activeItem = false;

        auto leftArrowBox = new ImageBox(containerThreeBox->firstBox, new Image("bell_arrow_left_W_M"));
        leftArrowBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
        leftArrowBox->setMinimumSizeToFitImage();
        leftArrowBox->setVisible(leftArrowVisible);
        leftArrowBox->setEdges(RectangleEdge::None);
        containerThreeBox->firstBox->setMinimumSize(leftArrowBox->widgetMinimumArea.w,
                                                    leftArrowBox->widgetMinimumArea.h);

        containerThreeBox->centerBox = new HBox(containerThreeBox);
        containerThreeBox->centerBox->setEdges(RectangleEdge::None);
        containerThreeBox->centerBox->setAlignment(
            Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        containerThreeBox->centerBox->setMaximumSize(container::image::width, container::image::height);

        auto imageBox = new ImageBox(containerThreeBox->centerBox, new Image(image, ImageTypeSpecifier::W_G));
        imageBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        imageBox->setMinimumSizeToFitImage();
        imageBox->setVisible(true);

        containerThreeBox->lastBox = new HBox(containerThreeBox);
        containerThreeBox->lastBox->setAlignment(Alignment(Alignment::Vertical::Bottom));
        containerThreeBox->lastBox->setEdges(RectangleEdge::None);
        containerThreeBox->lastBox->activeItem = false;

        auto rightArrowBox = new ImageBox(containerThreeBox->lastBox, new Image("bell_arrow_right_W_M"));
        rightArrowBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
        rightArrowBox->setMinimumSizeToFitImage();
        rightArrowBox->setVisible(rightArrowVisible);
        rightArrowBox->setEdges(RectangleEdge::None);
        containerThreeBox->lastBox->setMinimumSize(rightArrowBox->widgetMinimumArea.w,
                                                   rightArrowBox->widgetMinimumArea.h);

        auto containerSeparator = new VBox(this, 0, 0, container::width, container::separator::height);
        containerSeparator->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        containerSeparator->setEdges(RectangleEdge::None);
        containerSeparator->activeItem = false;

        auto containerText = new VBox(this, 0, 0, container::text::width, container::text::height);
        containerText->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        containerText->setEdges(RectangleEdge::None);
        containerText->activeItem = false;

        auto textBox = new Text(containerText, 0, 0, 0, 0);
        textBox->setMinimumSize(container::text::width, container::text::height);
        textBox->setAlignment(Alignment::Horizontal::Center);
        textBox->setTextType(TextType::MultiLine);
        textBox->setEditMode(EditMode::Browse);
        textBox->setFont(style::window::font::verybiglight);
        textBox->setRichText(utils::translate(description));

        this->resizeItems();
    }

    auto OnBoardingShortcutsLayoutClassic::getLayout() -> Item *
    {
        return this;
    }
}; // namespace gui
