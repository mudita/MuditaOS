// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include "application-settings-new/models/DateAndTimeModel.hpp"

namespace gui
{
    class ListView;
    class ChangeDateAndTimeWindow : public AppWindow
    {
      public:
        explicit ChangeDateAndTimeWindow(app::Application *app);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        void sendRtcUpdateTimeMessage();

        std::shared_ptr<DateAndTimeModel> dateAndTimeModel;
        std::shared_ptr<utils::time::FromTillDate> fromTillDate;
        ListView *list = nullptr;
    };
} /* namespace gui */
