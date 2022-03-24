// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsLayoutWindow.hpp"
#include <common/options/OptionBellMenu.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/windows/BellFinishedWindow.hpp>

namespace
{
    inline constexpr auto arrowsOuterMargin = 48U;
} // namespace

namespace gui
{
    BellSettingsLayoutWindow::BellSettingsLayoutWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::LayoutWindowPresenter::Presenter> &&presenter,
        const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsLayoutWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        auto layouts = presenter->getLayouts();
        spinner      = new WidgetSpinner(this, {layouts.begin(), layouts.end()}, Boundaries::Fixed);
        spinner->setSize(style::window_width, style::window_height);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        auto selectedLayout = presenter->getSelectedLayout();
        spinner->setCurrentValue(selectedLayout);

        createArrowsOverlay(0, 0, style::window_width, style::window_height);
        arrowLeft->setVisible(!spinner->isAtMin());
        arrowRight->setVisible(!spinner->isAtMax());

        spinner->onValueChanged = [this](const auto &) {
            arrowLeft->setVisible(!spinner->isAtMin());
            arrowRight->setVisible(!spinner->isAtMax());
        };
        setFocusItem(spinner);
    }

    void BellSettingsLayoutWindow::createArrowsOverlay(unsigned int x, unsigned y, unsigned int w, unsigned int h)
    {
        auto arrowsOverlay = new HBox{this, x, y, w, h};
        arrowsOverlay->setEdges(gui::RectangleEdge::None);
        arrowLeft = new Image("bell_arrow_left_W_M");
        arrowLeft->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        arrowLeft->activeItem = false;
        arrowLeft->setEdges(RectangleEdge::None);
        arrowLeft->setMargins(Margins{arrowsOuterMargin, 0, 0, 0});
        arrowsOverlay->addWidget(arrowLeft);

        arrowRight = new Image("bell_arrow_right_W_M");
        arrowRight->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        arrowRight->activeItem = false;
        arrowRight->setEdges(RectangleEdge::None);
        arrowRight->setMargins(Margins{0, 0, arrowsOuterMargin, 0});

        auto rectFiller = new gui::Rect(arrowsOverlay,
                                        0U,
                                        0U,
                                        arrowsOverlay->getWidth() - arrowLeft->getWidth() - arrowRight->getWidth() -
                                            (2 * arrowsOuterMargin),
                                        arrowsOverlay->getHeight());

        rectFiller->setMaximumSize(arrowsOverlay->getWidth(), arrowsOverlay->getHeight());
        rectFiller->setEdges(RectangleEdge::None);

        arrowsOverlay->addWidget(arrowRight);
    }
    bool BellSettingsLayoutWindow::onInput(const InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->setLayout(spinner->getCurrentValue());
            application->switchWindow(
                window::bell_finished::defaultName,
                BellFinishedWindowData::Factory::create("circle_success_big", gui::name::window::main_window));
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
