#pragma once

#include "../messages/AudioMessage.hpp"
#include "Audio/decoder/decoder.hpp"

#include <optional>

class Service;

namespace AudioServiceAPI
{
    /*! @brief Starts playback operation.
     *
     * @param serv - requesting service.
     * @param playbackType - type of playback.
     * If none, request would still be valid and default volume would be used.
     * @param fileName - name of the file.
     * @return           Standard service-api return code. Success if suitable.
     */
    audio::RetCode PlaybackStart(sys::Service *serv,
                                 const audio::PlaybackType &playbackType,
                                 const std::string &fileName);
    audio::RetCode RecordingStart(sys::Service *serv, const std::string &fileName);
    audio::RetCode RoutingStart(sys::Service *serv);
    audio::RetCode RoutingRecordCtrl(sys::Service *serv, bool enable);
    audio::RetCode RoutingMute(sys::Service *serv, bool enable);
    audio::RetCode RoutingSpeakerPhone(sys::Service *serv, bool enable);
    audio::RetCode RoutingHeadset(sys::Service *serv, bool enable);
    /// Stops current audio operation.
    ///
    /// Stops current audio operation when it's valid and not idle.
    /// @param serv requesting service
    /// @param stopVec vector that contains playback types to be stopped.
    /// When no vector is passed it stops current operation.
    /// When stop vector is passed it stops current operation only if it's type is contained in the vector.
    /// @return Standard service-api return code. Success if suitable.
    audio::RetCode Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec = {});
    audio::RetCode Pause(sys::Service *serv);
    audio::RetCode Resume(sys::Service *serv);
    std::optional<audio::Tags> GetFileTags(sys::Service *serv, const std::string &fileName);
    // audio::RetCode AdjustVolume(sys::Service *serv, const int &step = 0);
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
