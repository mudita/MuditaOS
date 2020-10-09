#include "SimSwitchWindow.hpp"
#include <string>
#include <module-utils/Utils.hpp>
#include <application-settings-new/ApplicationSettings.hpp>

namespace style
{
    namespace password
    {
        constexpr uint32_t x = 165;
        constexpr uint32_t y = 415;
        constexpr uint32_t w = 150;
        constexpr uint32_t h = 75;
    } // namespace password
} // namespace style
namespace gui
{
    SimSwitchWindow::SimSwitchWindow(app::Application *app) : Dialog(app, gui::window::name::switch_sim, Dialog::Meta())
    {
        update();
        password = new Text(this, style::password::x, style::password::y, style::password::w, style::password::h);
        password->setTextType(TextType::SINGLE_LINE);
        password->setEditMode(EditMode::EDIT);
        password->setInputMode(new InputMode({InputMode::digit}));
        password->setUnderline(true);
        setFocusItem(password);

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        topBar->setActive(TopBar::Elements::SIM, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
    }

    void SimSwitchWindow::update(Meta &meta)
    {
        password->activatedCallback = [=](Item &) -> bool {
            getApplication()->returnToPreviousWindow();
            return meta.action();
        };
        password->clear();

        update();
    }

    void SimSwitchWindow::update()
    {
        meta.title  = utils::translateI18("app_settings_sim_setup");
        auto prompt = utils::translateI18("app_settings_enter_pin_setup_to_sim");

        std::string switch_card;
        switch (Store::GSM::get()->selected) {
        case Store::GSM::SIM::SIM1:
            switch_card = utils::translateI18("app_settings_network_sim2");
            break;
        case Store::GSM::SIM::SIM2:
            switch_card = utils::translateI18("app_settings_network_sim1");
            break;
        default:
            LOG_ERROR("SimSwitchDialog no active sim");
        }
        utils::findAndReplaceAll(prompt, "$SIM_CARD$", switch_card);
        utils::findAndReplaceAll(meta.title, "$SIM_CARD$", switch_card);
        meta.text = prompt;
        meta.icon = "settings_sim_setup";
        Dialog::update(meta);
    }
} // namespace gui