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

    auto local_decorate(Label *el)
    {
        el->setSize( style::window_width, style::window::label::default_h);
        el->penFocusWidth     = 2;
        el->penWidth     = 0;
    }

    auto button_with_action(Item *parent, Label *child, std::string text, std::function<bool(Item &)> foo, bool visible = true)
    {
        child = new gui::Label();
        local_decorate(child);
        child->setText(text.c_str());
        child->activatedCallback = foo;
        child->visible = visible;
        parent->addWidget(child);
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

        box = new VBox(this, style::margins::huge, title->offset_h() + style::margins::huge, style::window_width - 2*style::margins::huge, style::window_height - 200);

        // TODO this should be Selector too...
        button_with_action(box,on_button,"On", [&](Item &) -> bool {
            LOG_DEBUG("send clicked: %d", bt_is_on);
            bt_is_on = !bt_is_on;
            auto message = std::make_shared<message::bt::OnOff>(bt_is_on ? message::bt::OnOff::State::On : message::bt::OnOff::State::Off);
            sys::Bus::SendUnicast(message, "ServiceBT", this->getApplication(), 10000);

//            if (send_button != nullptr) {
//                send_button->visible=bt_is_on;
//            }
//            if (power_button != nullptr) {
//                power_button->visible=bt_is_on;
//            }
//            if (chanel_button != nullptr) {
//                chanel_button->visible =bt_is_on;
//            }
//            if (pattern_button != nullptr) {
//                pattern_button->visible = bt_is_on;
//            }
            return true;
        });

        button_with_action(box, send_button, "Execute", [&](Item &) -> bool {
            LOG_DEBUG("send clicked");
            auto message = std::make_shared<message::bt::Test>(
                power_button->getVal(), chanel_button->getVal(), pattern_button->getVal());
            sys::Bus::SendUnicast(message, "ServiceBT", this->getApplication(), 10000);
            return true;
        }, visible = true);

        {
            using namespace bt::tx_test;

            power_button = new Selector<bt::tx_test::Power>(
                "Power: ", {bt::tx_test::Power::Max, bt::tx_test::Power::Med, bt::tx_test::Power::Min});
            local_decorate(power_button);
            box->addWidget(power_button);

            chanel_button = new Selector<bt::tx_test::Chanel>(
                "Chanel: ", {bt::tx_test::Chanel::f2402, bt::tx_test::Chanel::f2442, bt::tx_test::Chanel::f2480});
            local_decorate(chanel_button);
            box->addWidget(chanel_button);

            pattern_button = new Selector<bt::tx_test::Pattern>("Pattern: ",
                                                                {Pattern::All1,
                                                                 Pattern::All0});
//                                                                 Pattern::FF00,
//                                                                 Pattern::FOFO,
//                                                                 Pattern::PN15,
//                                                                 Pattern::PN9,
//                                                                 Pattern::Zebra});
            local_decorate(pattern_button);
            box->addWidget(pattern_button);
        };

        setFocusItem(box);
        LOG_INFO("...");
    }

    bool BtWindow::onInput(const InputEvent &inputEvent)
    {
        auto ret = AppWindow::onInput(inputEvent);
        // box->resizeItems();
        return ret;
    }

    void BtWindow::destroyInterface()
    {
        erase();
    }

    void BtWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

} // namespace gui
