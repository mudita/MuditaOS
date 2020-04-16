#include "SearchBox.hpp"

namespace gui
{

    gui::Text *searchBox(gui::Item *parent, const std::string &header, const std::string &icon)
    {
        auto inputField = new Text(nullptr,
                                   phonebookStyle::search::horizontalBox::inputField::x,
                                   phonebookStyle::search::horizontalBox::inputField::y,
                                   phonebookStyle::search::horizontalBox::inputField::w,
                                   phonebookStyle::search::horizontalBox::inputField::h);
        inputField->setTextType(Text::TextType::SINGLE_LINE);
        inputField->setEditMode(Text::EditMode::EDIT);
        inputField->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        inputField->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));
        inputField->setFont(style::window::font::mediumbold);

        auto horizontalBox = new HBox(parent,
                                      phonebookStyle::search::horizontalBox::x,
                                      phonebookStyle::search::horizontalBox::y,
                                      phonebookStyle::search::horizontalBox::w,
                                      phonebookStyle::search::horizontalBox::h);
        horizontalBox->addWidget(inputField);
        horizontalBox->addWidget(new Image(nullptr,
                                           phonebookStyle::search::horizontalBox::searchTop::x,
                                           phonebookStyle::search::horizontalBox::searchTop::y,
                                           phonebookStyle::search::horizontalBox::searchTop::w,
                                           phonebookStyle::search::horizontalBox::searchTop::h,
                                           icon));

        horizontalBox->setPenWidth(phonebookStyle::search::horizontalBox::penWidth);
        horizontalBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        const RectangleEdgeFlags edges = RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES;
        const Alignment alignment      = Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM);
        auto l                         = new Label(parent,
                           phonebookStyle::search::searchHeader::x,
                           phonebookStyle::search::searchHeader::y,
                           phonebookStyle::search::searchHeader::w,
                           phonebookStyle::search::searchHeader::h);

        l->setFont(style::window::font::small);
        // l->setFilled(false);
        // l->setBorderColor(ColorFullBlack);
        l->setEdges(edges);
        l->setText(header);
        l->setAlignment(alignment);

        return inputField;
    }
} // namespace gui
