// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellFactoryReset.hpp"
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <Application.hpp>

namespace gui
{
    BellFactoryReset::BellFactoryReset(app::ApplicationCommon *app,
                                       std::unique_ptr<AbstractPowerOffPresenter> presenter)
        : WindowWithTimer(app, name), presenter(std::move(presenter))
    {
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            this->presenter->powerOff(sys::CloseReason::FactoryReset);
            return true;
        };
    }

    void BellFactoryReset::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        if (icon == nullptr) {
            icon = new Icon(this, 0, 0, style::window_width, style::window_height, "circle_success_big", {});
            icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        }
    }
    bool BellFactoryReset::onInput(const InputEvent &)
    {
        return false;
    }

} // namespace gui
