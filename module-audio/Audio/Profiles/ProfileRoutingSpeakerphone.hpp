/*
 *  @file ProfileRoutingSpeakerphone.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 02.08.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_PROFILEROUTINGSPEAKERPHONE_HPP
#define PUREPHONE_PROFILEROUTINGSPEAKERPHONE_HPP

#include "Profile.hpp"

namespace audio
{

    class ProfileRoutingSpeakerphone : public Profile
    {
      public:
        ProfileRoutingSpeakerphone(std::function<int32_t()> callback, float volume, float gain)
            : Profile("Routing Speakerphone",
                      Type::RoutingSpeakerphone,
                      bsp::AudioDevice::Format{
                          .sampleRate_Hz = 16000,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       bsp::AudioDevice::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono),
                          .outputVolume = volume,
                          .inputGain    = gain,
                          .inputPath    = bsp::AudioDevice::InputPath::Microphone,
                          .outputPath   = bsp::AudioDevice::OutputPath::Loudspeaker},
                      bsp::AudioDevice::Type::Audiocodec,
                      callback)
        {}
    };

} // namespace audio

#endif // PUREPHONE_PROFILEROUTINGSPEAKERPHONE_HPP
