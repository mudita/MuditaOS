#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"

#include "i18/i18.hpp"

#include "BtWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"

#include <Style.hpp>

#include <list>
#include <utility>

namespace gui
{

    BtWindow::BtWindow(app::Application *app) : AppWindow(app, "Bluetooth")
    {
        buildInterface();
    }

    void BtWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void BtWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_settings_bt"));

        LOG_INFO("Create box layout");

//        const Position send_offset = 100;
//        const Position margin = 5;
//        uint32_t no_elements = 3;

        send_button = new gui::Label(this, 0, title->offset_h(), style::window_width, style::window::label::default_h, "Start");
        send_button->activatedCallback = [&](Item &) -> bool {
            LOG_DEBUG("send clicked");
            auto message = std::make_shared<BtOnOff>(stamp_message);
            sys::Bus::SendUnicast(message, "ServiceBT", getApplication());
            return true;
        };
        send_button->penFocusWidth = 2;

        LOG_INFO("ui built");
        setFocusItem(send_button);
        LOG_INFO("...");
    }

    bool BtWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void BtWindow::destroyInterface()
    {
        erase();
    }

    void BtWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

} // namespace gui
