// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 *  @file IdleOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_IDLEOPERATION_HPP
#define PUREPHONE_IDLEOPERATION_HPP

#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"

namespace audio
{

    class IdleOperation : public Operation
    {
      public:
        IdleOperation(const char *file);

        ~IdleOperation() = default;

        audio::RetCode Start([[maybe_unused]] audio::AsyncCallback callback, audio::Token token) override final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode Stop() override final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode Pause() override final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode Resume() override final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode SendEvent(std::shared_ptr<Event> evt) override final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode SwitchProfile(const Profile::Type type) override final
        {
            return audio::RetCode::Success;
        }

        audio::RetCode SetOutputVolume(float vol) override final;

        audio::RetCode SetInputGain(float gain) override final;

        Position GetPosition() override final
        {
            return 0.0;
        }
    };

} // namespace audio

#endif // PUREPHONE_IDLEOPERATION_HPP
