// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmModel.hpp"

#include <apps-common/AsyncTask.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>

#include <functional>

namespace app
{
    class Application;
}

namespace app
{
    class AlarmModel : public AbstractAlarmModel, public AsyncCallbackReceiver
    {
      public:
        explicit AlarmModel(Application *app);

        bool isActive() const override;
        void setAlarmTime(time_t time) override;
        time_t getAlarmTime() const override;
        void activate(bool value) override;
        void update() override;

      private:
        enum class State
        {
            Invalid,
            InitInProgress,
            Valid
        };

        void updateAlarm(const AlarmEventRecord &alarm);
        Application *app{};
        AlarmEventRecord cachedRecord;
        State state{State::Invalid};

        std::function<bool(sys::ResponseMessage *)> responseCallback;
    };
} // namespace app
