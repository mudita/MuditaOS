/*
 *  @file IdleOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "IdleOperation.hpp"

namespace audio
{

    IdleOperation::IdleOperation([[maybe_unused]] const char *file) : Operation{true}
    {
    }

    audio::RetCode IdleOperation::SetOutputVolume(float vol)
    {
        return RetCode::Success;
    }

    audio::RetCode IdleOperation::SetInputGain(float gain)
    {
        return RetCode::Success;
    }
} // namespace audio
