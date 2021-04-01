// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioMessage.hpp"

#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <Audio/decoder/Decoder.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

class Service;
namespace audio
{
    struct Tags;
} // namespace audio
namespace sys
{
    class Service;
} // namespace sys

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
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioStartPlaybackResponse
     */
    bool PlaybackStart(sys::Service *serv, const audio::PlaybackType &playbackType, const std::string &fileName);

    /**
     * @brief Starts recording. Asynchronous call.
     *
     * @param serv Requesting service.
     * @param fileName Path to file where recording is to be saved.
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioStartRecordingResponse
     */
    bool RecordingStart(sys::Service *serv, const std::string &fileName);
    /**
     * @brief Starts routing. Asynchronous call.
     *
     * @param serv Requesting service.
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioStartRoutingResponse
     */
    bool RoutingStart(sys::Service *serv);
    /**
     * @brief Stops playback operations by type. Asynchronous call.
     *
     * @param serv Requesting service
     * @param stopVec Playback types to be stopped.
     *  When stop vector is passed it stops current operation only if it's type is contained in the vector.
     *  Otherwise does not have effect.
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioStopResponse for all stopped inputs
     */
    bool Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec);
    /**
     * @brief Stops playback operation.
     *
     * @param serv Requesting service
     * @param token Identifier of related operation
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioStopResponse for all stopped inputs
     */
    bool Stop(sys::Service *serv, const audio::Token &token);
    /**
     * @brief Stops all active playback operation. Stopped operations cannot be resumed. Asynchronous call.
     *
     * @param serv Requesting service
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioStopResponse for all stopped inputs
     */
    bool StopAll(sys::Service *serv);
    /**
     * @brief Pauses playback operation. Can be resumed by Resume(). Asynchronous call.
     *
     * @param serv Requesting service
     * @param token Identifier of related operation
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioPauseResponse
     */
    bool Pause(sys::Service *serv, const audio::Token &token);
    /**
     * @brief Resumes paused operation. Asynchronous call.
     *
     * @param serv Requesting service
     * @param token Identifier of related operation
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioResumeResponse
     */
    bool Resume(sys::Service *serv, const audio::Token &token);
    /**
     * @brief Sends audio event
     * @param serv Requesting service.
     * @param evt Event to be sent.
     * @return True is request has been sent successfully, false otherwise
     *   Response will come as message AudioSendEventResponse
     */
    bool SendEvent(sys::Service *serv, std::shared_ptr<audio::Event> evt);
    /**
     * @brief Sends audio event
     * @param serv Requesting service.
     * @param evt Event to be sent.
     * @param state Optional parameter to request.
     * @return True is request has been sent successfully, false otherwise
     *   Response will come as message AudioSendEventResponse
     */
    bool SendEvent(sys::Service *serv,
                   audio::EventType evt,
                   audio::Event::DeviceState state = audio::Event::DeviceState::Connected);
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
    /** @brief Key pressed handler.
     *
     * @param serv - requesting service.
     * @param step - step that will be added to current volume.
     * @return True if request has been sent successfully, false otherwise
     *  Response will come as message AudioKeyPressedResponse
     */
    bool KeyPressed(sys::Service *serv, const int step);

    bool BluetoothVolumeChanged(sys::Service *serv, const uint8_t volume);
}; // namespace AudioServiceAPI
