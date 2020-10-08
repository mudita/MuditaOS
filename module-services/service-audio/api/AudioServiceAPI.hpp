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
     * @brief Starts playback operation. Asynchronous call.
     *
     * @param serv Requesting service.
     * @param playbackType Type of playback.
     * @param fileName Name of the file.
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioStartPlaybackResponse
     */
    bool PlaybackStart(sys::Service *serv, const audio::PlaybackType &playbackType, const std::string &fileName);

    /**
     * @brief Starts recording. Asynchronous call.
     *
     * @param serv Requesting service.
     * @param fileName Path to file where recording is to be saved.
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioStartRecordingResponse
     */
    bool RecordingStart(sys::Service *serv, const std::string &fileName);
    /**
     * @brief Starts routing. Asynchronous call.
     *
     * @param serv Requesting service.
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioStartRoutingResponse
     */
    bool RoutingStart(sys::Service *serv);
    bool RoutingMute(sys::Service *serv, bool enable);
    bool RoutingSpeakerPhone(sys::Service *serv, bool enable);
    bool RoutingHeadset(sys::Service *serv, bool enable);
    /**
     * @brief Stops playback operations by type. Asynchronous call.
     *
     * @param serv Requesting service
     * @param stopVec Playback types to be stopped.
     *  When stop vector is passed it stops current operation only if it's type is contained in the vector.
     *  Otherwise does not have effect.
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioStopResponse for all stopped inputs
     */
    bool Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec);
    /**
     * @brief Stops playback operation.
     *
     * @param serv Requesting service
     * @param token Identifier of related operation
     * @return audio::RetCode standard service-api return code
     *  Response will come as message AudioStopResponse
     */
    bool Stop(sys::Service *serv, const audio::Token &token);
    /**
     * @brief Stops all active playback operation. Stopped operations cannot be resumed. Asynchronous call.
     *
     * @param serv Requesting service
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioStopResponse for all stopped inputs
     */
    bool StopAll(sys::Service *serv);
    /**
     * @brief Pauses playback operation. Can be resumed by Resume(). Asynchronous call.
     *
     * @param serv Requesting service
     * @param token Identifier of related operation
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioPauseResponse
     */
    bool Pause(sys::Service *serv, const audio::Token &token);
    /**
     * @brief Resumes paused operation. Asynchronous call.
     *
     * @param serv Requesting service
     * @param token Identifier of related operation
     * @return True is request has been sent successfully, false otherwise
     *  Response will come as message AudioResumeResponse
     */
    bool Resume(sys::Service *serv, const audio::Token &token);
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
     * @return Standard service-api return code. Success if suitable.
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
     * @return Standard service-api return code. Success if suitable.
     */
    template <typename T>
    audio::RetCode SetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              const T value,
                              const audio::PlaybackType &playbackType = audio::PlaybackType::None,
                              const audio::Profile::Type &profileType = audio::Profile::Type::Idle);

    bool GetSettingAsync(sys::Service *serv,
                         const audio::Setting &setting,
                         const audio::Profile::Type &profileType = audio::Profile::Type::Idle,
                         const audio::PlaybackType &playbackType = audio::PlaybackType::None);

    bool KeyPressed(sys::Service *serv, const int step, const std::vector<audio::PlaybackType> &typesToMute);

}; // namespace AudioServiceAPI
