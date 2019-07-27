/*
 *  @file ProfileRecordingHeadset.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 24.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PROFILERECORDINGHEADSET_HPP
#define PUREPHONE_PROFILERECORDINGHEADSET_HPP

#include "Profile.hpp"

class ProfileRecordingHeadset : public Profile {
public:
    ProfileRecordingHeadset(std::function<int32_t()> callback, float gain) : Profile(
            "Recording Headset",Type::RecordingHeadset, 0.0, gain, 0, 0, bsp::AudioDevice::Type::Audiocodec, callback) {}
};


#endif //PUREPHONE_PROFILERECORDINGHEADSET_HPP
