// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/TimeModel.hpp>
#include <apps-common/BasePresenter.hpp>

namespace app
{
    class AbstractTimeModel;
    class ApplicationCommon;
} // namespace app

namespace app::relaxation
{
    class RelaxationPausedContract
    {
      public:
        class View
        {
          public:
            virtual ~View()                                                 = default;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        };

        class Presenter : public BasePresenter<RelaxationPausedContract::View>
        {
          public:
            virtual void handleUpdateTimeEvent() = 0;
            virtual void onBeforeShow()          = 0;
        };
    };

    class RelaxationPausedPresenter : public RelaxationPausedContract::Presenter
    {
        std::unique_ptr<AbstractTimeModel> timeModel;

        void handleUpdateTimeEvent() override;
        void onBeforeShow() override;

      public:
        RelaxationPausedPresenter(std::unique_ptr<AbstractTimeModel> timeModel);
    };
} // namespace app::relaxation
