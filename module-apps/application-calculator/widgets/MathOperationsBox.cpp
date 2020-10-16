#include "MathOperationsBox.hpp"
#include "application-calculator/widgets/CalculatorStyle.hpp"
#include <gui/widgets/Label.hpp>
#include <cassert>

namespace gui
{
    MathOperationsBox::MathOperationsBox(
        gui::Item *parent, int offsetTop, uint32_t width, uint32_t height, uint32_t cellWidth, uint32_t cellHeight)
        : GridLayout(parent, style::window::default_left_margin, offsetTop, width, height, {cellWidth, cellHeight})
    {
        assert(parent != nullptr);
        parent->addWidget(this);

        grid.x = cellWidth;
        grid.y = cellHeight;

        fillInTheGrid();
    }

    void MathOperationsBox::fillInTheGrid()
    {
        std::array<const std::string, style::calculator::grid_cells> math_operations = {
            "",
            style::calculator::symbols::strings::plus,
            "",
            style::calculator::symbols::strings::multiplication,
            style::calculator::symbols::strings::equals,
            style::calculator::symbols::strings::division,
            "",
            style::calculator::symbols::strings::minus,
            ""};

        for (const auto &symbol : math_operations) {
            auto cell = new Label(this);
            cell->setSize(grid.x, grid.y);
            cell->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            cell->setEdges(gui::RectangleEdge::None);
            cell->setText(symbol);
            cell->setFont(style::window::font::largelight);
            this->addWidget(cell);
        }
    }

} // namespace gui
