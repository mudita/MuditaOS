// Check includes, some are not neccessary
#include "windows/BellHomeScreenWindow.hpp"
#include "layouts/HomeScreenLayoutClassicWithAmPm.hpp"
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
    HomeScreenLayoutClassicWithAmPm::HomeScreenLayoutClassicWithAmPm(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
    }

    auto HomeScreenLayoutClassicWithAmPm::buildInterface() -> void
    {
        using namespace bellMainStyle;

        amPmText = new TextFixedSize(nullptr, 0, 0, 0, 0);
        amPmText->setMaximumSize(style::bell_home_screen::bottom_desc_w, style::bell_home_screen::bottom_desc_h);
        amPmText->setFont(mainWindow::bottomDescription::font_small);
        amPmText->setEdges(RectangleEdge::None);
        amPmText->activeItem = false;
        amPmText->drawUnderline(false);
        amPmText->setText("AM/PM");

        statusBox->setItems(battery, amPmText, nullptr);
    }

    auto HomeScreenLayoutClassicWithAmPm::setViewState(app::home_screen::ViewState state) -> void
    {
        HomeScreenLayoutClassic::setViewState(state);

        if (state == app::home_screen::ViewState::Deactivated || state == app::home_screen::ViewState::Activated ||
            state == app::home_screen::ViewState::AlarmSnoozed) {
            amPmText->setVisible(true);
        }
        else {
            amPmText->setVisible(false);
        }
    }
}; // namespace gui
