// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <application-settings/models/FromTimeToTimeModel.hpp>

namespace gui
{
    class ListView;
    class VBox;
    class NightshiftWindow : public AppWindow
    {
      public:
        explicit NightshiftWindow(app::Application *app);

      private:
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        VBox *body = nullptr;
        std::shared_ptr<utils::time::FromTillDate> fromTillDate;
        gui::ListView *list = nullptr;
        std::shared_ptr<FromTimeToTimeModel> timeModel;
    };
} /* namespace gui */
