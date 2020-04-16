#include "SearchBox.hpp"
#include <BoxLayout.hpp>
#include "i18/i18.hpp"
#include <Image.hpp>

namespace gui
{
    constexpr uint32_t default_x = style::window::default_left_margin;
    constexpr uint32_t default_w =
        style::window_width - style::window::default_left_margin - style::window::default_right_margin;
    namespace searchHeader
    {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = 127;
        constexpr uint32_t w = 86;
        constexpr uint32_t h = 20;
    } // namespace searchHeader

    namespace horizontalBox
    {
        constexpr uint32_t x        = style::window::default_left_margin;
        constexpr uint32_t y        = 153;
        constexpr uint32_t w        = default_w;
        constexpr uint32_t h        = 33;
        constexpr uint32_t penWidth = 2;
        namespace searchTop
        {
            constexpr uint32_t x = default_w;
            constexpr uint32_t y = 1;
            constexpr uint32_t w = 32;
            constexpr uint32_t h = 32;
        } // namespace searchTop
        namespace inputField
        {
            constexpr uint32_t x = 1;
            constexpr uint32_t y = 1;
            constexpr uint32_t w = default_w - searchTop::w;
            constexpr uint32_t h = 33;
        } // namespace inputField
    }     // namespace horizontalBox
} // namespace gui

namespace gui
{

    gui::Text *searchBox(gui::Item *parent, const std::string &header, const std::string &icon)
    {
        auto inputField = new Text(nullptr,
                                   horizontalBox::inputField::x,
                                   horizontalBox::inputField::y,
                                   horizontalBox::inputField::w,
                                   horizontalBox::inputField::h);
        inputField->setTextType(Text::TextType::SINGLE_LINE);
        inputField->setEditMode(Text::EditMode::EDIT);
        inputField->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        inputField->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));
        inputField->setFont(style::window::font::mediumbold);

        auto horizontalBox = new HBox(parent, horizontalBox::x, horizontalBox::y, horizontalBox::w, horizontalBox::h);
        horizontalBox->addWidget(inputField);
        horizontalBox->addWidget(new Image(nullptr,
                                           horizontalBox::searchTop::x,
                                           horizontalBox::searchTop::y,
                                           horizontalBox::searchTop::w,
                                           horizontalBox::searchTop::h,
                                           icon));

        horizontalBox->setPenWidth(horizontalBox::penWidth);
        horizontalBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        const RectangleEdgeFlags edges = RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES;
        const Alignment alignment      = Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM);
        auto l = new Label(parent, searchHeader::x, searchHeader::y, searchHeader::w, searchHeader::h);

        l->setFont(style::window::font::small);
        l->setEdges(edges);
        l->setText(header);
        l->setAlignment(alignment);

        return inputField;
    }
} // namespace gui
