// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorMainWindow.hpp"
#include <data/CalculatorInputProcessorText.hpp>
#include <widgets/CalculatorStyle.hpp>

namespace gui
{

    CalculatorMainWindow::CalculatorMainWindow(app::ApplicationCommon *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void CalculatorMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_calculator_title_main"));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, calc::symbols::strings::equals_str());
        bottomBar->setText(gui::BottomBar::Side::LEFT, calc::symbols::strings::decimal_separator_str());
        bottomBar->setFont(BottomBar::Side::LEFT, style::window::font::largelight);

        mathOperationInput = new gui::Text(this,
                                           style::calculator::window::input_margin,
                                           style::calculator::window::input_offset_top,
                                           style::calculator::window::input_width,
                                           style::calculator::window::input_height);
        mathOperationInput->setEdges(gui::RectangleEdge::Bottom);
        mathOperationInput->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        mathOperationInput->setFont(style::window::font::supersizemelight);
        mathOperationInput->setPenFocusWidth(style::window::default_border_focus_w);
        mathOperationInput->setPenWidth(style::window::default_border_rect_no_focus);
        mathOperationInput->setEditMode(gui::EditMode::Edit);

        mathBox = new gui::MathOperationsBox(this,
                                             style::calculator::window::math_box_offset_top,
                                             style::window::default_body_width,
                                             style::calculator::window::math_box_height,
                                             style::calculator::window::math_box_cell_width,
                                             style::calculator::window::math_box_cell_height);
        addWidget(mathBox);
        setFocusItem(mathOperationInput);

        setupInputProcessing();
    }

    void CalculatorMainWindow::setupInputProcessing()
    {
        inputProcessor = std::make_unique<calc::InputProcessorText>(gsl::make_strict_not_null(mathOperationInput));

        mathOperationInput->setInputMode(new InputMode({InputMode::digit}));
        mathOperationInput->inputCallback = [&](Item &, const InputEvent &event) {
            return inputProcessor->handle(event);
        };
    }

    bool CalculatorMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        return false;
    }

} // namespace gui
