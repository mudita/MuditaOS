/*
 *  @file ProfileRecordingOnBoardMic.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 24.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PROFILERECORDINGONBOARDMIC_HPP
#define PUREPHONE_PROFILERECORDINGONBOARDMIC_HPP

#include "Profile.hpp"

namespace audio {

    class ProfileRecordingOnBoardMic : public Profile {
    public:
        ProfileRecordingOnBoardMic(std::function<int32_t()> callback, float gain) : Profile(
                "Recording On Board Microphone", Type::RecordingBuiltInMic, 0.0, gain, 0, 0,
                bsp::AudioDevice::Type::Audiocodec, callback) {}
    };

}
#endif //PUREPHONE_PROFILERECORDINGONBOARDMIC_HPP
