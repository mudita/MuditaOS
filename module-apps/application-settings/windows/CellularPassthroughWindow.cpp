#include "CellularPassthroughWindow.hpp"
#include <module-utils/i18/i18.hpp>
#include <module-bsp/bsp/cellular/bsp_cellular.hpp>
#include "Label.hpp"
#include <cassert>

using namespace bsp::cellular::USB;

namespace gui
{

    CellularPassthroughWindow::CellularPassthroughWindow(app::Application *app)
        : AppWindow(app, window::cellular_passthrough::window_name)
    {
        buildInterface();
    }

    void CellularPassthroughWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void CellularPassthroughWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_settings_cellular_passthrough"));

        layout = new VBox(this, 0, title->offset_h(), style::window_width, 8 * style::window::label::big_h);
        layout->setPenFocusWidth(style::window::default_border_no_focus_w);

        statusHbox = new gui::HBox(layout,
                                   style::window::default_left_margin,
                                   0,
                                   style::window_width -
                                       (style::window::default_left_margin + style::window::default_right_margin),
                                   style::window::label::big_h);
        statusHbox->setPenWidth(0);
        statusHbox->activeItem = false;

        currentStateConst = new Label(nullptr, 0, 0, 0, style::window::label::big_h, "State:");
        style::window::decorate(currentStateConst);
        currentStateConst->setFont(style::window::font::big);
        currentStateConst->setSize(currentStateConst->getTextNeedSpace(), currentStateConst->getHeight());

        currentStateVariable = new Label(nullptr, 0, 0, 0, style::window::label::big_h);
        style::window::decorate(currentStateVariable);
        currentStateVariable->setFont(style::window::font::bigbold);
        currentStateVariable->setAlignment(gui::Alignment::Horizontal::Right);
        currentStateVariable->setSize(statusHbox->getWidth() - currentStateConst->getTextNeedSpace(),
                                      currentStateVariable->getHeight());

        statusHbox->addWidget(currentStateConst);
        statusHbox->addWidget(currentStateVariable);

        addSeparator(layout);

        // init label buttons
        // enable buttons
        passNormal = new Label(nullptr, 0, 0, layout->getWidth(), style::window::label::big_h);
        style::window::decorateOption(passNormal);
        passNormal->setPadding(Padding(style::window::default_left_margin, 0, style::window::default_right_margin, 0));
        passNormal->setText(window::cellular_passthrough::passNormalDescription);
        passNormal->activatedCallback = [=](gui::Item &) {
            if (set(PassthroughState::ENABLED, BootPinState::NORMAL_BOOT)) {
                setWindowState(State::PASS_NORMAL);
            }
            else {
                setWindowState(State::FAIL);
            }
            return true;
        };
        layout->addWidget(passNormal);

        passDFU = new Label(nullptr, 0, 0, layout->getWidth(), style::window::label::big_h);
        style::window::decorateOption(passDFU);
        passDFU->setPadding(Padding(style::window::default_left_margin, 0, style::window::default_right_margin, 0));
        passDFU->setText(window::cellular_passthrough::passDFUDescription);
        passDFU->activatedCallback = [=](gui::Item &) {
            if (set(PassthroughState::ENABLED, BootPinState::FIRMWARE_UPGRADE)) {
                setWindowState(State::PASS_DFU);
            }
            else {
                setWindowState(State::FAIL);
            }
            return true;
        };
        layout->addWidget(passDFU);

        // disable buttons
        noPass = new Label(nullptr, 0, 0, layout->getWidth(), style::window::label::big_h);
        style::window::decorateOption(noPass);
        noPass->setPadding(Padding(style::window::default_left_margin, 0, style::window::default_right_margin, 0));
        noPass->setText(window::cellular_passthrough::noPassDescription);
        noPass->activatedCallback = [=](gui::Item &) {
            if (set(PassthroughState::DISABLED, BootPinState::NORMAL_BOOT)) {
                setWindowState(State::NO_PASS);
            }
            else {
                setWindowState(State::FAIL);
            }
            return true;
        };
        layout->addWidget(noPass);

        layout->resizeItems();

        setFocusItem(layout);

        // init note
        auto noteSeparator = new Rect(layout, 0, 0, layout->getWidth(), 50);
        noteSeparator->setPenWidth(style::window::default_border_no_focus_w);
        noteSeparator->activeItem = false;

        note = new Label(layout,
                         2 * style::window::default_left_margin,
                         0,
                         layout->getWidth() -
                             2 * (style::window::default_left_margin + style::window::default_right_margin),
                         style::window::label::default_h);
        note->setFont(style::window::font::small);
        note->setFilled(true);
        note->setTextColor(gui::ColorFullWhite);
        note->setFillColor(gui::ColorFullBlack);
        note->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        note->activeItem = false;

        // get initial state
        setWindowState(getInitialState());
    }

    void CellularPassthroughWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void CellularPassthroughWindow::invalidate() noexcept
    {
        layout               = nullptr;
        passNormal           = nullptr;
        passDFU              = nullptr;
        noPass               = nullptr;
        statusHbox           = nullptr;
        currentStateConst    = nullptr;
        currentStateVariable = nullptr;
        note                 = nullptr;
    }

    void CellularPassthroughWindow::addSeparator(VBox *layout)
    {
        assert(layout != nullptr);
        uint16_t height = style::window::label::small_h * 2;
        auto separatorLayout =
            new VBox(nullptr,
                     style::window::default_left_margin,
                     0,
                     (style::window_width - (style::window::default_left_margin + style::window::default_right_margin)),
                     height);
        separatorLayout->setPenWidth(0);

        auto separatorUpper = new Rect(nullptr, 0, 0, separatorLayout->getWidth(), height / 2);
        auto separatorLower = new Rect(nullptr, 0, 0, separatorLayout->getWidth(), height / 2);
        separatorUpper->setBorderColor(gui::ColorFullBlack);
        separatorLower->setBorderColor(gui::ColorFullBlack);
        separatorUpper->setPenWidth(1);
        separatorLower->setPenWidth(1);
        separatorUpper->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        separatorLower->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

        separatorLayout->addWidget(separatorUpper);
        separatorLayout->addWidget(separatorLower);
        separatorLayout->activeItem = false;
        layout->addWidget(separatorLayout);
    }

    CellularPassthroughWindow::State CellularPassthroughWindow::getInitialState()
    {
        auto pass    = getPassthrough();
        auto bootPin = getBootPin();

        auto ret = State::UNKNOWN;

        if (pass == PassthroughState::ENABLED) {
            if (bootPin == BootPinState::FIRMWARE_UPGRADE) {
                ret = State::PASS_DFU;
            }
            else {
                ret = State::PASS_NORMAL;
            }
        }
        else if (pass == PassthroughState::DISABLED) {
            ret = State::NO_PASS;
        }
        return ret;
    }

    bool CellularPassthroughWindow::set(PassthroughState pass_to_set, BootPinState dfu_to_set)
    {
        setPassthrough(pass_to_set);
        setBootPin(dfu_to_set);
        // in order to really enter DFU mode, modem needs be restarted

        // verify if new state has been applied with "Software Input On Field" loopback function
        vTaskDelay(pdMS_TO_TICKS(wait_for_pin_change_state));
        return (getPassthrough() == pass_to_set && getBootPin() == dfu_to_set);
    }

    void CellularPassthroughWindow::setWindowState(CellularPassthroughWindow::State state)
    {
        note->setVisible(false);

        if (state == State::PASS_NORMAL) {
            currentStateVariable->setText(window::cellular_passthrough::passNormalLabel);
            passNormal->setVisible(false);
            passDFU->setVisible(false);
            noPass->setVisible(true);
            if (previousState == State::PASS_DFU) {
                note->setText(window::cellular_passthrough::noPassNote);
                note->setVisible(true);
            }
            // no additional action is needed
        }
        else if (state == State::PASS_DFU) {
            currentStateVariable->setText(window::cellular_passthrough::passDFULabel);
            passNormal->setVisible(false);
            passDFU->setVisible(false);
            noPass->setVisible(true);
            // here modem needs be reset. PWK_KEY reset (aka. nice reset) should be enough
            note->setText(window::cellular_passthrough::passDFUNote);
            note->setVisible(true);
        }
        else if (state == State::NO_PASS) {
            currentStateVariable->setText(window::cellular_passthrough::noPassLabel);
            passNormal->setVisible(true);
            passDFU->setVisible(true);
            noPass->setVisible(false);
            if (previousState == State::PASS_DFU) {
                // reset with a reset key or even Vcc reset i.e. force reset
                note->setText(window::cellular_passthrough::noPassNote);
                note->setVisible(true);
            }
        }
        else if (state == State::FAIL) {
            currentStateVariable->setText("FAIL");
        }
        statusHbox->resizeItems();
        layout->setVisible(true);

        previousState = state;
    }

} // namespace gui
