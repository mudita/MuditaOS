#pragma once

#include "../messages/AudioMessage.hpp"
#include "Audio/decoder/decoder.hpp"

#include <optional>

class Service;

/**
 * Audio service API.
 */
namespace AudioServiceAPI
{
    /**
     * @brief Starts playback operation.
     *
     * @param serv Requesting service.
     * @param playbackType Type of playback.
     * @param fileName Name of the file.
     * @return audio::Handle Handle to operation to be used in subsequent operations.
     */
    audio::Handle PlaybackStart(sys::Service *serv,
                                const audio::PlaybackType &playbackType,
                                const std::string &fileName);
    /**
     * @brief Starts recording.
     *
     * @param serv Requesting service.
     * @param fileName Path to file where recording is to be saved.
     * @return audio::Handle Handle to operation to be used in subsequent operations.
     */
    audio::Handle RecordingStart(sys::Service *serv, const std::string &fileName);
    /**
     * @brief Starts routing.
     *
     * @param serv Requesting service.
     * @return audio::Handle Handle to operation to be used in subsequent operations.
     */
    audio::Handle RoutingStart(sys::Service *serv);
    audio::RetCode RoutingMute(sys::Service *serv, bool enable);
    audio::RetCode RoutingSpeakerPhone(sys::Service *serv, bool enable);
    audio::RetCode RoutingHeadset(sys::Service *serv, bool enable);
    /**
     * @brief Stops playback operations by type.
     *
     * @param serv Requesting service
     * @param stopVec Playback types to be stopped.
     *  When stop vector is passed it stops current operation only if it's type is contained in the vector.
     *  Otherwise does not have effect.
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec);
    /**
     * @brief Stops playback operation.
     *
     * @param serv Requesting service
     * @param handle Handle to controlled operation
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Stop(sys::Service *serv, const audio::Handle &handle);
    /**
     * @brief Stops all active playback operation. Stopped operations cannot be resumed.
     *
     * @param serv Requesting service
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode StopAll(sys::Service *serv);
    /**
     * @brief Pauses playback operation. Can be resumed by Resume()
     *
     * @param serv Requesting service
     * @param handle Handle to controlled operation
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Pause(sys::Service *serv, const audio::Handle &handle);
    /**
     * @brief Resumes paused operation.
     *
     * @param serv Requesting service
     * @param handle Handle to controlled operation
     * @return audio::RetCode standard service-api return code
     */
    audio::RetCode Resume(sys::Service *serv, const audio::Handle &handle);
    /**
     * @brief Attempts to parse audio file for metatags.
     *
     * @param serv Requesting service.
     * @param fileName Path to file to be parsed.
     * @return audio::Tags on success, std::nullopt on failure
     */
    std::optional<audio::Tags> GetFileTags(sys::Service *serv, const std::string &fileName);
    /** @brief Gets settings. Current profile is taken by default.
     *
     * @param serv - requesting service.
     * @param setting - setting to be set eg. Gain, volume etc.
     * @param value - requested value.
     * @param profileType - selected profile type.
     * @param playbackType -  type of playback. Not used when profileType is different than playback.
     * @return           Standard service-api return code. Success if suitable.
     */
    template <typename T>
    audio::RetCode GetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              T &value,
                              const audio::PlaybackType &playbackType = audio::PlaybackType::None,
                              const audio::Profile::Type &profileType = audio::Profile::Type::Idle);
    /** @brief Sets settings. Current profile is taken by default.
     *
     * @param serv - requesting service.
     * @param setting - setting to be set eg. Gain, volume etc.
     * @param value - value to be set.
     * @param profileType - selected profile type.
     * @param playbackType -  type of playback. Not used when profileType is different than playback.
     * @return           Standard service-api return code. Success if suitable.
     */
    template <typename T>
    audio::RetCode SetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              const T value,
                              const audio::PlaybackType &playbackType = audio::PlaybackType::None,
                              const audio::Profile::Type &profileType = audio::Profile::Type::Idle);

}; // namespace AudioServiceAPI
