#pragma once

#include "layouts/HomeScreenLayoutClassic.hpp"

#include "widgets/BellBattery.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithAmPm : public HomeScreenLayoutClassic
    {
      public:
        HomeScreenLayoutClassicWithAmPm(std::string name);

        auto setViewState(app::home_screen::ViewState state) -> void override;

      protected:
        auto buildInterface() -> void override;

        TextFixedSize *amPmText{};
    };
}; // namespace gui
