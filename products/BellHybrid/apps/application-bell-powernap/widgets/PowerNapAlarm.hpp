// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-audio/Audio/AudioCommon.hpp>

namespace app
{
    class Application;
}

namespace app::powernap
{
    class PowerNapAlarm
    {
      public:
        virtual ~PowerNapAlarm() = default;

        virtual void start() = 0;
        virtual void stop()  = 0;
    };

    class PowerNapAlarmImpl : public PowerNapAlarm
    {
        app::Application *app{};
        audio::Token token;

        void start() override;
        void stop() override;

      public:
        explicit PowerNapAlarmImpl(app::Application *app);
        void registerAudioStream(audio::Token _token);
    };

} // namespace app::powernap
