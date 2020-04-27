#include "AppWindow.hpp"
#include "Application.hpp"
#include <i18/i18.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <Style.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>

using namespace style::header;

namespace gui
{

    AppWindow::AppWindow(app::Application *app, std::string name, uint32_t id) : Window(name, id), application{app}
    {
        setSize(style::window_width, style::window_height);
    }

    AppWindow::~AppWindow()
    {
        LOG_INFO("deleting window: %s", name.c_str());
        destroyInterface();
    }

    void AppWindow::destroyInterface()
    {
        children.remove(bottomBar);
        children.remove(topBar);
        children.remove(title);
        delete title;
        delete bottomBar;
        delete topBar;
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
        title->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_TOP));
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
            title->setVisible(true);
            title->setText(text);
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
            return application->increaseCurrentVolume();
        }
        case KeyCode::KEY_VOLDN: {
            return application->decreaseCurrentVolume();
        }
        case KeyCode::KEY_RF: {
            return returnToPreviousView();
        }
        default:
            break;
        }

        return false;
    }

    bool AppWindow::returnToPreviousView()
    {
        auto prevWindow = application->getPrevWindow();
        if (prevWindow == gui::name::window::no_window) {
            LOG_INFO("Back to previous application");
            application->cleanPrevWindw();
            sapm::ApplicationManager::messageSwitchPreviousApplication(application);
        }
        else {
            LOG_INFO("Back to previous window %s", prevWindow.c_str());
            application->switchWindow(prevWindow, gui::ShowMode::GUI_SHOW_RETURN);
        }
        return true;
    }

    void AppWindow::textModeShowCB(const UTF8 &text)
    {
        if (bottomBar == nullptr) {
            return;
        }
        bottomBar->setText(BottomBar::Side::CENTER, text);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        bottomBar->getText(BottomBar::Side::CENTER);
    }

    bool AppWindow::textSelectSpecialCB()
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

} /* namespace gui */
