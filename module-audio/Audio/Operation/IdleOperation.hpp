// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"

namespace audio
{

    class IdleOperation : public Operation
    {
      public:
        explicit IdleOperation([[maybe_unused]] const char *file);

        ~IdleOperation() = default;

        audio::RetCode Start(audio::Token token) final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode Stop() final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode Pause() final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode Resume() final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode SwitchProfile(const Profile::Type type) final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode SetOutputVolume(float vol) final;

        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final
        {
            return 0.0;
        }
    };

} // namespace audio
