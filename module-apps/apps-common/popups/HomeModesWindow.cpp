// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HomeModesWindow.hpp"
#include "widgets/ModesBox.hpp"
#include "data/PopupData.hpp"

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/Arc.hpp>
#include <PhoneModes/Common.hpp>

namespace gui
{
    HomeModesWindow::HomeModesWindow(app::ApplicationCommon *app, const std::string &name) : WindowWithTimer(app, name)
    {
        buildInterface();
    }

    void HomeModesWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setVisible(false);
        statusBar->setVisible(false);

        modesBox = new ModesBox(this, 0, style::window::modes::top_offset);
    }

    void HomeModesWindow::rebuild()
    {}

    void HomeModesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        const auto popupData = dynamic_cast<ModesPopupData *>(data);
        if (popupData != nullptr) {
            const auto currentMode       = popupData->getPhoneMode();
            const auto currentFlightMode = popupData->getFlightMode();
            modesBox->update(currentMode, currentFlightMode);
        }
    }

    bool HomeModesWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            return false;
        }

        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
