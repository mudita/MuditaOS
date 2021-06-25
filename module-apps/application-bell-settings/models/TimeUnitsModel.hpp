// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/TimeSetSpinner.hpp>
#include <apps-common/Application.hpp>
#include <apps-common/SimpleInternalModel.hpp>
#include <module-gui/gui/widgets/SideListItemProvider.hpp>

namespace gui
{
    class TimeUnitsModel : public app::SimpleInternalModel<gui::SideListItem *>, public gui::SideListItemProvider
    {
      public:
        explicit TimeUnitsModel(app::Application *app);
        ~TimeUnitsModel();

        auto clearData() -> void;
        auto saveData() -> void;
        auto loadData() -> void;
        auto createData() -> void;

        auto getCurrentIndex() -> unsigned int;
        auto getItem(gui::Order order) -> gui::SideListItem * override;
        auto requestRecordsCount() -> unsigned int override;

      private:
        app::Application *application       = nullptr;
        gui::TimeSetSpinner *timeSetSpinner = nullptr;

        void sendRtcUpdateTimeMessage(time_t newTime);
    };
} // namespace gui
