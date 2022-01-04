// Check includes, some are not neccessary
#include "windows/BellHomeScreenWindow.hpp"
#include "layouts/HomeScreenLayoutClassicWithBattery.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/SnoozeTimer.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    HomeScreenLayoutClassicWithBattery::HomeScreenLayoutClassicWithBattery(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
    }
    auto HomeScreenLayoutClassicWithBattery::buildInterface() -> void
    {
        battery->setBatteryPercentMode(BatteryPercentMode::Static);
    }

    bool HomeScreenLayoutClassicWithBattery::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        return true;
    }
}; // namespace gui
