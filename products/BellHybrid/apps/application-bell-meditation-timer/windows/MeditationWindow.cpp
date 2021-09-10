// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationWindow.hpp"
#include "../data/MeditationSwitchData.hpp"

#include <InputEvent.hpp>

namespace gui
{

    MeditationWindow::MeditationWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        inputDisabled = false;
    }

    bool MeditationWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputDisabled) {
            if (inputEvent.is(gui::KeyCode::KEY_RF) || inputEvent.is(gui::KeyCode::KEY_PND)) {
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void MeditationWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            return;
        }

        if (auto switchData = dynamic_cast<meditation::MeditationSwitchData *>(data); data != nullptr) {
            item = switchData->getMeditationItem();
        }
    }

    void MeditationWindow::gotoWindow(std::string name)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<meditation::MeditationSwitchData>(item);
        application->switchWindow(name, std::move(data));
    }

    void MeditationWindow::disableInput()
    {
        inputDisabled = true;
    }
} // namespace gui
