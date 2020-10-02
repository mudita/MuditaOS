#include "NetworkWindow.hpp"

#include <i18/i18.hpp>
#include "OptionsStyle.hpp"
#include <application-settings-new/ApplicationSettings.hpp>

namespace gui
{
    NetworkWindow::NetworkWindow(app::Application *app) : OptionWindow(app, gui::window::name::network)
    {
        operatorsOn = false;
    }
    void NetworkWindow::onBeforeShow(ShowMode m, SwitchData *d)
    {
        rebuildOptList();
    }
    auto NetworkWindow::netOptList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        auto sim    = Store::GSM::get()->selected;
        auto simStr = utils::translateI18("app_settings_network_sim1");
        if (sim == Store::GSM::SIM::SIM2) {
            simStr = utils::translateI18("app_settings_network_sim2");
        }
        optList.emplace_back(std::make_unique<gui::NetworkOption>(
            utils::translateI18("app_settings_network_active_card") + ":" + simStr,
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::change_settings, nullptr);
                return true;
            },
            [=](gui::Item &item) {
                auto nlb = dynamic_cast<gui::Label *>(&item);
                if (nullptr != nlb) {
                    if (item.focus) {
                        this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    else {
                        this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                }
                return true;
            },
            this));
        optList.emplace_back(std::make_unique<gui::NetworkOption>(
            utils::translateI18("app_settings_network_operator_auto_select"),
            [=](gui::Item &item) {
                operatorsOn = !operatorsOn;
                rebuildOptList();
                return true;
            },
            nullptr,
            nullptr,
            operatorsOn ? NetworkArrow::On : NetworkArrow::Off));
        if (operatorsOn) {
            optList.emplace_back(gui::Option{utils::translateI18("app_settings_network_all_operators"),
                                             [=](gui::Item &item) {
                                                 this->application->switchWindow(gui::window::name::all_operators,
                                                                                 nullptr);
                                                 return true;
                                             },
                                             gui::Arrow::Enabled});
        }
        optList.emplace_back(gui::Option{
            utils::translateI18("app_settings_network_import_contacts_from_sim_card"), [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::import_contacts, nullptr);
                return true;
            }});

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));

        topBar->setActive(TopBar::Elements::SIGNAL, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);
        return optList;
    }
    void NetworkWindow::rebuildOptList()
    {
        clearOptions();
        addOptions(netOptList());
    }
    auto NetworkOption::build() const -> Item *
    {
        auto *label = new gui::Label(nullptr,
                                     style::window::default_left_margin,
                                     0,
                                     style::window_width - 2 * style::window::default_left_margin,
                                     style::window::label::big_h,
                                     text);
        style::window::decorateOption(label);
        label->activatedCallback = activatedCallback;
        std::string img;
        switch (arrow) {
        case NetworkArrow::Enabled:
            img = "right_label_arrow";
            break;
        case NetworkArrow::Border:
            img = "right_label_arrow_border";
            break;
        case NetworkArrow::On:
            img = "right_label_on";
            break;
        case NetworkArrow::Off:
            img = "right_label_off";
            break;
        default:
            break;
        }

        if (!img.empty()) {
            new gui::Image(label, style::option::arrow_position_x, style::option::arrow_positon_y, 0, 0, img);
        }
        label->focusChangedCallback = focusCb;
        return label;
    }
} // namespace gui
