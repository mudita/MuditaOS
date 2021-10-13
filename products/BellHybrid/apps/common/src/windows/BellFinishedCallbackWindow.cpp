// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common/windows/BellFinishedCallbackWindow.hpp"
#include "common/data/BellFinishedCallbackWindowSwitchData.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{

    BellFinishedCallbackWindow::BellFinishedCallbackWindow(app::ApplicationCommon *app, const std::string &name)
        : WindowWithTimer(app, name)
    {
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            if (this->finishCallback) {
                this->finishCallback();
            }
            return true;
        };
    }

    void BellFinishedCallbackWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
        icon->text->setFont(style::window::font::verybiglight);
    }

    bool BellFinishedCallbackWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            if (this->finishCallback) {
                this->finishCallback();
            }
            return true;
        }
        return false;
    }

    void BellFinishedCallbackWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);

        if (auto metadata = dynamic_cast<BellFinishedCallbackWindowSwitchData *>(data)) {
            icon->image->set(metadata->icon);
            icon->text->setRichText(metadata->text);
            finishCallback = metadata->finishCallback;
        }
    }

} // namespace gui
