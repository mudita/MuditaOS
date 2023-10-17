// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <popups/WindowWithTimer.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class BluetoothInfoPopup : public WindowWithTimer
    {
      private:
        Icon *infoIcon = nullptr;
        void closePopups();

      public:
        BluetoothInfoPopup(app::ApplicationCommon *app, const std::string &name);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} /* namespace gui */
