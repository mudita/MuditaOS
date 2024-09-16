// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "layouts/UpdateInstructionLayoutClassic.hpp"
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <i18n/i18n.hpp>
#include <Style.hpp>
#include <TextFixedSize.hpp>

namespace
{
    namespace container
    {
        constexpr auto width{544U};
        constexpr auto height{436U};
        constexpr auto top_margin{20U};

        namespace centerBox
        {
            constexpr auto width{448U};
            constexpr auto height{container::height};
        } // namespace centerBox

        namespace imageBox
        {
            constexpr auto width{centerBox::width};
            constexpr auto height{120U};
        } // namespace imageBox

        namespace title
        {
            constexpr auto height{56U};
            constexpr auto width{centerBox::width};
            constexpr auto font{style::window::font::large};
            constexpr auto bottom_margin{32U};
        } // namespace title

        namespace instruction
        {
            constexpr auto maxNumberOfLines{3};
            constexpr auto width{centerBox::width - 40U};
            constexpr auto minHeight{43U};
            constexpr auto maxHeight{minHeight * maxNumberOfLines};
            constexpr auto bottom_margin{14U};

            namespace number
            {
                constexpr auto width{36U};
                constexpr auto font{style::window::font::verybiglight};
            } // namespace number

            namespace text
            {
                constexpr auto width{instruction::width - number::width};
                constexpr auto font{style::window::font::verybiglight};
            } // namespace text
        }     // namespace instruction
    } // namespace container
} // namespace

namespace gui
{
    UpdateInstructionLayoutClassic::UpdateInstructionLayoutClassic(const UTF8 &image,
                                                                   const UTF8 &title,
                                                                   const Instruction &instruction,
                                                                   bool leftArrowVisible,
                                                                   bool rightArrowVisible)
        : UpdateInstructionLayoutProvider(image, title, leftArrowVisible, rightArrowVisible),
          VBox(nullptr, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h)
    {
        buildInterface(instruction);
    }

    auto UpdateInstructionLayoutClassic::buildInterface(const Instruction &instruction) -> void
    {
        setAlignment(Alignment::Horizontal::Center);
        setEdges(rectangle_enums::RectangleEdge::None);

        auto containerThreeBox = new HThreeBox<HBox, VBox, HBox>(this);
        containerThreeBox->setMinimumSize(container::width, container::height);
        containerThreeBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        containerThreeBox->setMargins(Margins(0, container::top_margin, 0, 0));
        containerThreeBox->setEdges(RectangleEdge::None);

        // -------------------------- left box -------------------------------------------
        containerThreeBox->firstBox = new HBox(containerThreeBox);
        containerThreeBox->firstBox->setAlignment(Alignment(Alignment::Vertical::Center));
        containerThreeBox->firstBox->setEdges(RectangleEdge::None);
        containerThreeBox->firstBox->activeItem = false;

        leftArrowBox = new ImageBox(nullptr, new Image("bell_arrow_left_W_M"));
        leftArrowBox->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        leftArrowBox->setMinimumSizeToFitImage();
        leftArrowBox->setVisible(leftArrowVisible);
        leftArrowBox->setEdges(RectangleEdge::None);
        containerThreeBox->firstBox->setMinimumSize(leftArrowBox->widgetMinimumArea.w,
                                                    leftArrowBox->widgetMinimumArea.h);

        containerThreeBox->firstBox->addWidget(leftArrowBox);

        // -------------------------- center box -------------------------------------------
        containerThreeBox->centerBox = new VBox(containerThreeBox);
        containerThreeBox->centerBox->setEdges(RectangleEdge::None);
        containerThreeBox->centerBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        containerThreeBox->centerBox->setMinimumSize(container::centerBox::width, container::centerBox::height);
        containerThreeBox->centerBox->setMaximumSize(container::centerBox::width, container::centerBox::height);

        imageBox = new ImageBox(nullptr, new Image(image, ImageTypeSpecifier::W_G));
        imageBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        imageBox->setMinimumSizeToFitImage();
        imageBox->setMaximumSize(container::imageBox::width, container::imageBox::height);
        imageBox->setVisible(true);

        titleBox = new Text(nullptr, 0, 0, 0, 0);
        titleBox->setMinimumSize(container::title::width, container::title::height);
        titleBox->setMaximumSize(container::title::width, container::title::height);
        titleBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        titleBox->setMargins(Margins(0, 0, 0, container::title::bottom_margin));
        titleBox->setTextType(TextType::SingleLine);
        titleBox->setEditMode(EditMode::Browse);
        titleBox->setFont(container::title::font);
        titleBox->setRichText(utils::translate(title));

        containerThreeBox->centerBox->addWidget(imageBox);
        containerThreeBox->centerBox->addWidget(titleBox);

        for (const auto &point : instruction) {
            auto instructionContainer = new HBox(nullptr);
            instructionContainer->setEdges(RectangleEdge::None);
            instructionContainer->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Top));
            instructionContainer->setMargins(Margins(0, 0, 0, container::instruction::bottom_margin));
            instructionContainer->setMinimumSize(container::instruction::width, container::instruction::minHeight);
            instructionContainer->setMaximumSize(container::instruction::width, container::instruction::maxHeight);

            auto numberBox = new Text(nullptr, 0, 0, 0, 0);
            numberBox->setMinimumSize(container::instruction::number::width, container::instruction::minHeight);
            numberBox->setAlignment(Alignment::Horizontal::Left);
            numberBox->setTextType(TextType::MultiLine);
            numberBox->setEditMode(EditMode::Browse);
            numberBox->setFont(container::instruction::number::font);
            numberBox->setRichText(point.first);

            auto textBox = new Text(nullptr, 0, 0, 0, 0);
            textBox->setMinimumSize(container::instruction::text::width, container::instruction::minHeight);
            textBox->setMaximumSize(container::instruction::text::width, container::instruction::maxHeight);
            textBox->setAlignment(Alignment::Horizontal::Left);
            textBox->setTextType(TextType::MultiLine);
            textBox->setEditMode(EditMode::Browse);
            textBox->setFont(container::instruction::text::font);
            textBox->setRichText(utils::translate(point.second));

            InstructionPoint instructionPoint{
                .container = instructionContainer, .textBox = textBox, .numberBox = numberBox};
            instructionPoints.push_back(instructionPoint);
        }

        for (const auto &point : instructionPoints) {
            point.container->addWidget(point.numberBox);
            point.container->addWidget(point.textBox);
            containerThreeBox->centerBox->addWidget(point.container);
        }

        // -------------------------- right box -------------------------------------------
        containerThreeBox->lastBox = new HBox(containerThreeBox);
        containerThreeBox->lastBox->setAlignment(Alignment(Alignment::Vertical::Center));
        containerThreeBox->lastBox->setEdges(RectangleEdge::None);
        containerThreeBox->lastBox->activeItem = false;

        rightArrowBox = new ImageBox(nullptr, new Image("bell_arrow_right_W_M"));
        rightArrowBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        rightArrowBox->setMinimumSizeToFitImage();
        rightArrowBox->setVisible(rightArrowVisible);
        rightArrowBox->setEdges(RectangleEdge::None);
        containerThreeBox->lastBox->setMinimumSize(rightArrowBox->widgetMinimumArea.w,
                                                   rightArrowBox->widgetMinimumArea.h);

        containerThreeBox->lastBox->addWidget(rightArrowBox);

        this->resizeItems();
    }

    auto UpdateInstructionLayoutClassic::getLayout() -> Item *
    {
        return this;
    }
}; // namespace gui
