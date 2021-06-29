// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/models/DateAndTimeModel.hpp>
#include <AppWindow.hpp>

namespace gui
{
    namespace window::name
    {
        inline constexpr auto change_date_and_time = "ChangeDateAndTime";
    }

    class ListView;
    class ChangeDateAndTimeWindow : public AppWindow
    {
      public:
        explicit ChangeDateAndTimeWindow(app::Application *app,
                                         std::string name = window::name::change_date_and_time);

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
