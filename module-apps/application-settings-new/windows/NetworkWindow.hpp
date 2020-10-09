#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace gui
{

    namespace window
    {
        inline const std::string network_window = "Network";
    };

    class NetworkWindow : public OptionWindow
    {
      private:
        bool operatorsOn;
        auto netOptList() -> std::list<gui::Option>;
        void rebuildOptList();

      public:
        NetworkWindow(app::Application *app);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
        void showSwitchSim();
        void switchSim();
    };
} // namespace gui
