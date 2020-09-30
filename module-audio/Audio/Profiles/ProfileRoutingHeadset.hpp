/*
 *  @file ProfileRoutingHeadset.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 02.08.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRoutingHeadset : public Profile
    {
      public:
        ProfileRoutingHeadset(std::function<int32_t()> callback, Volume volume, Gain gain)
            : Profile("Routing Headset",
                      Type::RoutingHeadset,
                      bsp::AudioDevice::Format{
                          .sampleRate_Hz = 16000,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       bsp::AudioDevice::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono),
                          .outputVolume = static_cast<float>(volume),
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = bsp::AudioDevice::InputPath::Headphones,
                          .outputPath   = bsp::AudioDevice::OutputPath::Headphones},
                      bsp::AudioDevice::Type::Audiocodec,
                      callback)
        {}
    };

} // namespace audio

