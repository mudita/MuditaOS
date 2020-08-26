#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <BoxLayout.hpp>
#include <service-bt/messages/BtInject.hpp>

namespace gui
{
    template <class T> class Selector : public Label
    {
        std::string base_text;
        std::list<T> values;

        bool handleActivatedCallback() 
        {
            if (values.size() == 0) {
                return false;
            }
            std::rotate(values.begin(), std::next(values.begin()), values.end());
            setText((base_text + std::string(c_str(values.front()))).c_str());
            return true;
        }

      public:
        Selector(std::string base_text, std::list<T> v) : values(v)
        {
            activatedCallback = [&](Item &) -> bool {
                return handleActivatedCallback();
                            };
            if( values.size() == 0 ) 
            {
                return;
            }
            setText((base_text + std::string(c_str(values.front()))).c_str());
            // visible = false;
        }

        auto getVal()
        {
            return values.front();
        }
    };

    class BtWindow : public AppWindow
    {
      protected:
        Label *on_button;
        Label *send_button;
        Selector<bt::tx_test::Power> *power_button;
        Selector<bt::tx_test::Chanel> *chanel_button;
        Selector<bt::tx_test::Pattern> *pattern_button;

        gui::Item *addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback);

        bool bt_is_on = false;
        VBox *box = nullptr;

      public:
        BtWindow(app::Application *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
        void set_navigation();
    };
} // namespace gui
