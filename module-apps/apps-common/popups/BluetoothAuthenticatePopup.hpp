// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Popups.hpp"
#include "data/PopupRequestParams.hpp"

#include <TextFixedSize.hpp>
#include <Icon.hpp>
#include <AppWindow.hpp>

namespace gui
{
    class BluetoothAuthenticatePopup : public AppWindow
    {
      public:
        BluetoothAuthenticatePopup(app::ApplicationCommon *app, const std::string &name);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void createTextAuthenticateInput();
        void createButtonsAuthenticateInput();

        BluetoothAuthenticateRequestParams *authenticateParams = nullptr;
        Icon *infoIcon                                         = nullptr;
        TextFixedSize *inputText                               = nullptr;
    };
} // namespace gui
