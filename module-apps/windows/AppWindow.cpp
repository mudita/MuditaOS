#include "AppWindow.hpp"
#include "Application.hpp"
#include "InputEvent.hpp"
#include <Style.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <i18/i18.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>

using namespace style::header;

namespace gui
{

    AppWindow::AppWindow(app::Application *app, std::string name) : Window(name), application{app}
    {
        setSize(style::window_width, style::window_height);
    }

    AppWindow::~AppWindow()
    {
        LOG_INFO("deleting window: %s", name.c_str());
        onClose();
    }

    void AppWindow::destroyInterface()
    {
        erase(bottomBar);
        erase(topBar);
        erase(title);
        title     = nullptr;
        bottomBar = nullptr;
        topBar    = nullptr;
    }

    void AppWindow::rebuild()
    {}

    void AppWindow::buildInterface()
    {
        bottomBar = new gui::BottomBar(this, 0, 599 - 50, 480, 50);
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, false);

        title = new gui::Label(this, 0, 52, 480, 52);
        title->setFilled(false);
        title->setFont(font::title);
        title->clear();
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        title->setEllipsis(Ellipsis::Right);
        title->visible = false;

        topBar = new gui::TopBar(this, 0, 0, 480, 50);
        topBar->setActive(TopBar::Elements::LOCK, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIGNAL, false);
    }

    bool AppWindow::batteryCharging(bool charging)
    {
        topBar->setBatteryCharging(charging);
        return true;
    }

    bool AppWindow::setSIM()
    {
        topBar->simSet();
        return true;
    }

    // updates battery level in the window
    bool AppWindow::updateBatteryLevel(uint32_t percentage)
    {
        // get old value of battery level, calcualte new level and comapre both
        // if they are different make a change and return true, otherwise return false;
        return topBar->setBatteryLevel(percentage);
    }
    // updates battery level in the window
    bool AppWindow::updateSignalStrength()
    {
        return topBar->updateSignalStrength();
    }

    bool AppWindow::updateTime(const UTF8 &timeStr)
    {
        topBar->setTime(timeStr);
        return true;
    }

    bool AppWindow::updateTime(const uint32_t &timestamp, bool mode24H)
    {
        topBar->setTime(timestamp, mode24H);
        return true;
    }

    void AppWindow::setTitle(const UTF8 &text)
    {
        if (title != nullptr) {
            title->setText(text);
            title->setVisible(text.length() != 0);
        }
        else {
            LOG_ERROR("cant set title - it doesn't exist!");
        }
    }

    std::list<DrawCommand *> AppWindow::buildDrawList()
    {
        return Window::buildDrawList();
    }

    bool AppWindow::onDatabaseMessage(sys::Message *msg)
    {
        return false;
    }

    bool AppWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (Window::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.state == InputEvent::State::keyReleasedLong && inputEvent.keyCode == gui::KeyCode::KEY_RF) {
            LOG_INFO("exit to main menu");
            sapm::ApplicationManager::messageSwitchApplication(
                application, app::name_desktop, gui::name::window::main_window, nullptr);
        }
        // process only if key is released
        if ((inputEvent.state != InputEvent::State::keyReleasedShort))
            return false;

        switch (inputEvent.keyCode) {
        case KeyCode::KEY_VOLUP: {
            application->increaseCurrentVolume();
            return true;
        }
        case KeyCode::KEY_VOLDN: {
            application->decreaseCurrentVolume();
            return true;
        }
        case KeyCode::KEY_RF: {
            application->returnToPreviousWindow();
            return true;
        }
        case KeyCode::KEY_TORCH: {
            application->toggleTorchAndColourTemps();
            return true;
        }
        default:
            break;
        }

        return false;
    }

    void AppWindow::bottomBarTemporaryMode(const UTF8 &text, bool emptyOthers)
    {
        bottomBarTemporaryMode(text, BottomBar::Side::LEFT, emptyOthers);
    }

    void AppWindow::bottomBarTemporaryMode(const UTF8 &text, BottomBar::Side side, bool emptyOthers)
    {
        if (bottomBar == nullptr) {
            return;
        }

        bottomBar->store();

        if (emptyOthers) {
            bottomBar->setText(BottomBar::Side::LEFT, "");
            bottomBar->setText(BottomBar::Side::CENTER, "");
            bottomBar->setText(BottomBar::Side::RIGHT, "");
        }

        switch (side) {
        case BottomBar::Side::LEFT:
            bottomBar->setText(BottomBar::Side::LEFT, text);
            break;
        case BottomBar::Side::CENTER:
            bottomBar->setText(BottomBar::Side::CENTER, text);
            break;
        case BottomBar::Side::RIGHT:
            bottomBar->setText(BottomBar::Side::RIGHT, text);
            break;
        }
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void AppWindow::bottomBarRestoreFromTemporaryMode()
    {
        if (bottomBar == nullptr) {
            return;
        }

        bottomBar->restore();
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void AppWindow::setBottomBarText(const UTF8 &text, BottomBar::Side side)
    {
        bottomBar->setText(side, text);
    }

    void AppWindow::clearBottomBarText(BottomBar::Side side)
    {
        bottomBar->setText(side, "");
    }

    bool AppWindow::selectSpecialCharacter()
    {
        return sapm::ApplicationManager::messageSwitchSpecialInput(
            application,
            std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Request, application->GetName()));
    }

    BoundingBox AppWindow::bodySize()
    {
        return {0,
                title->offset_h(),
                this->getWidth(),
                this->getHeight() - this->title->offset_h() - bottomBar->getHeight()};
    }

    void AppWindow::setBottomBarActive(BottomBar::Side side, bool value)
    {
        bottomBar->setActive(side, value);
    }

} /* namespace gui */
