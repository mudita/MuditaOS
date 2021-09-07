// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PhoneLockedInfoData.hpp"
#include <module-apps/apps-common/popups/WindowWithTimer.hpp>
#include <AppWindow.hpp>
#include <Text.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class PhoneLockedInfoWindow : public WindowWithTimer
    {
      public:
        PhoneLockedInfoWindow(app::Application *app, const std::string &name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

      private:
        PhoneLockedInfoData::Stage mStage = PhoneLockedInfoData::Stage::Idle;
        Icon *infoIcon                    = nullptr;
    };
} /* namespace gui */
