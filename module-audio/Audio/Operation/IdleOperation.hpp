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

        ~IdleOperation();

        audio::RetCode Start([[maybe_unused]] std::function<int32_t(AudioEvents event)> callback) override final
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

        int32_t Resume() override final
        {
            return 0;
        }

        int32_t SendEvent(const Event evt, const EventData *data = nullptr) override final
        {
            return 0;
        }

        int32_t SwitchProfile(const Profile::Type type) override final
        {
            return 0;
        }

        int32_t SetOutputVolume(float vol) override final;

        int32_t SetInputGain(float gain) override final;

        Position GetPosition() override final
        {
            return 0.0;
        }
    };

} // namespace audio

#endif // PUREPHONE_IDLEOPERATION_HPP
