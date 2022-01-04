#pragma once

#include "layouts/HomeScreenLayoutClassic.hpp"

#include "widgets/BellBattery.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithBattery : public HomeScreenLayoutClassic
    {
      public:
        HomeScreenLayoutClassicWithBattery(std::string name);

        auto buildInterface() -> void override;

      protected:
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;
    };
}; // namespace gui
