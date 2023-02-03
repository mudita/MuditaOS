// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioMessage.hpp"

#include <Audio/AudioCommon.hpp>
#include <Audio/decoder/Decoder.hpp>
#include <Audio/Profiles/Profile.hpp>

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
     * @param fileName Name of the file
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
     * @brief Sends audio event as multicast
     * @param serv Requesting service.
     * @param evt Event to be sent.
     */
    void SendEvent(sys::Service *serv, std::shared_ptr<audio::Event> evt);
    /**
     * @brief Sends audio event as multicast
     * @param serv Requesting service.
     * @param evt Event to be sent.
     * @param state Optional parameter to request.
     */
    void SendEvent(sys::Service *serv,
                   audio::EventType evt,
                   audio::Event::DeviceState state = audio::Event::DeviceState::Connected);

    /** @brief Sets vibrations setting state
     *
     * @param serv - requesting service.
     * @param settingState - state to be set.
     * @param playbackType -  type of playback.
     * @return Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetVibrationSetting(sys::Service *serv,
                                       audio::SettingState settingState,
                                       audio::PlaybackType playbackType);

    /** @brief Gets vibrations setting state
     *
     * @param serv - requesting service.
     * @param playbackType -  type of playback.
     * @return Requested setting state on success. std::nullopt on failure
     */
    std::optional<audio::SettingState> GetVibrationSetting(sys::Service *serv, audio::PlaybackType playbackType);

    /** @brief Sets vibrations level setting state
     *
     * @param serv - requesting service.
     * @param vibrationLevel - vibration level to be set.
     * @return Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetVibrationLevelSetting(sys::Service *serv, audio::VibrationLevel vibrationLevel);

    /** @brief Gets vibrations level setting state
     *
     * @param serv - requesting service.
     * @return Requested setting vibration level on success. std::nullopt on failure
     */
    std::optional<audio::VibrationLevel> GetVibrationLevelSetting(sys::Service *serv);

    /** @brief Sets system sound setting state
     *
     * @param serv - requesting service.
     * @param settingState - state to be set.
     * @param playbackType -  type of playback.
     * @return Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetSystemSoundSetting(sys::Service *serv,
                                         audio::SettingState settingState,
                                         audio::PlaybackType playbackType);

    /** @brief Gets system sound setting state
     *
     * @param serv - requesting service.
     * @param playbackType -  type of playback.
     * @return Requested setting state on success. std::nullopt on failure
     */
    std::optional<audio::SettingState> GetSystemSoundSetting(sys::Service *serv, audio::PlaybackType playbackType);

    /** @brief Sets sound setting state
     *
     * @param serv - requesting service.
     * @param settingState - state to be set.
     * @param playbackType -  type of playback.
     * @return Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetSoundSetting(sys::Service *serv,
                                   audio::SettingState settingState,
                                   audio::PlaybackType playbackType);

    /** @brief Gets sound setting state
     *
     * @param serv - requesting service.
     * @param playbackType -  type of playback. Not used when profileType is different than playback.
     * @return Requested setting state on success. std::nullopt on failure
     */
    std::optional<audio::SettingState> GetSoundSetting(sys::Service *serv, audio::PlaybackType playbackType);

    /** @brief Sets sound for chosen playback type
     *
     * @param serv - requesting service.
     * @param settingState - path to the file
     * @param playbackType -  type of playback.
     * @return Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetSound(sys::Service *serv, const std::string &filePath, audio::PlaybackType playbackType);

    /** @brief Gets sound for chosen playback type
     *
     * @param serv - requesting service.
     * @param playbackType -  type of playback.
     * @return requested path to the file. Empty string on failure
     */
    std::string GetSound(sys::Service *serv, audio::PlaybackType playbackType);

    /** @brief Sets volume level for chosen playback type
     *
     * @param serv - requesting service.
     * @param vol - volume level
     * @param playbackType -  type of playback.
     * @return Standard service-api return code. Success if suitable.
     */
    audio::RetCode SetVolume(sys::Service *serv, const audio::Volume vol, audio::PlaybackType playbackType);

    /** @brief Gets volume level for chosen playback type
     *
     * @param serv - requesting service.
     * @param playbackType -  type of playback.
     * @return requested volume level. std::nullopt on failure
     */
    std::optional<audio::Volume> GetVolume(sys::Service *serv, audio::PlaybackType playbackType);

    /** @brief Key pressed handler.
     *
     * @param serv - requesting service.
     * @param step - step that will be added to current volume.
     * @return True if request has been sent successfully, false otherwise
     * @note If volume popup should be shown then Application Manager is informed.
     */
    bool KeyPressed(sys::Service *serv, const int step);

    /** @brief Bluetooth A2DP volume changed handler.
     *
     * @param serv - requesting service.
     * @param volume - volume level
     * @return True if request has been sent successfully, false otherwise
     */
    bool BluetoothA2DPVolumeChanged(sys::Service *serv, const std::uint8_t volume);

    /** @brief Bluetooth HSP volume changed handler.
     *
     * @param serv - requesting service.
     * @param volume - volume level
     * @return True if request has been sent successfully, false otherwise
     */
    bool BluetoothHSPVolumeChanged(sys::Service *serv, const std::uint8_t volume);

    /** @brief Bluetooth HFP volume changed handler.
     *
     * @param serv - requesting service.
     * @param volume - volume level
     * @return True if request has been sent successfully, false otherwise
     */
    bool BluetoothHFPVolumeChanged(sys::Service *serv, const std::uint8_t volume);
}; // namespace AudioServiceAPI
