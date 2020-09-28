#pragma once

#include "../messages/AudioMessage.hpp"
#include "Audio/decoder/decoder.hpp"

#include <optional>

class Service;

namespace AudioServiceAPI
{
    /*! @brief Starts playback operation.
     *
     * @param serv Requesting service.
     * @param playbackType Type of playback.
     * @param fileName Name of the file.
     * @return audio::Handle Handle to operation to be used in subsequent operation.
     */
    audio::Handle PlaybackStart(sys::Service *serv,
                                const audio::PlaybackType &playbackType,
                                const std::string &fileName);
    audio::Handle RecordingStart(sys::Service *serv, const std::string &fileName);
    audio::Handle RoutingStart(sys::Service *serv);
    audio::RetCode RoutingRecordCtrl(sys::Service *serv, bool enable);
    audio::RetCode RoutingMute(sys::Service *serv, bool enable);
    audio::RetCode RoutingSpeakerPhone(sys::Service *serv, bool enable);
    audio::RetCode RoutingHeadset(sys::Service *serv, bool enable);
    audio::RetCode Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec = {});
    /*!
     * @brief Stops playback operation.
     *
     * @param serv Requesting service
     * @param handle Handle to controlled operation
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Stop(sys::Service *serv, const audio::Handle &handle);
    /*!
     * @brief Pauses playback operation. Can be resumed by Resume()
     *
     * @param serv Requesting service
     * @param handle Handle to controlled operation
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Pause(sys::Service *serv, const audio::Handle &handle);
    /*!
     * @brief Resumes paused operation.
     *
     * @param serv Requesting service
     * @param handle Handle to controlled operation
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Resume(sys::Service *serv, const audio::Handle &handle);

    std::optional<audio::Tags> GetFileTags(sys::Service *serv, const std::string &fileName);
    /*! @brief Gets settings. Current profile is taken by default.
     *
     * @param serv - requesting service.
     * @param setting - setting to be set eg. Gain, volume etc.
     * @param value - requested value.
     * @param profileType - selected profile type.
     * @param playbackType -  type of playback. Not used when profileType is different than playback.
     * @return           Standard service-api return code. Success if suitable.
     */
    audio::RetCode GetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              uint32_t &value,
                              const audio::Profile::Type &profileType = audio::Profile::Type::Idle,
                              const audio::PlaybackType &playbackType = audio::PlaybackType::None);
    /*! @brief Sets settings. Current profile is taken by default.
     *
     * @param serv - requesting service.
     * @param setting - setting to be set eg. Gain, volume etc.
     * @param value - value to be set.
     * @param profileType - selected profile type.
     * @param playbackType -  type of playback. Not used when profileType is different than playback.
     * @return           Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              const uint32_t value,
                              const audio::Profile::Type &profileType = audio::Profile::Type::Idle,
                              const audio::PlaybackType &playbackType = audio::PlaybackType::None);

}; // namespace AudioServiceAPI
