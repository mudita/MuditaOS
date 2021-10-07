// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioMux.hpp"
#include "Audio.hpp"

namespace audio
{
    namespace
    {
        const std::map<PlaybackType, uint8_t> PlaybackTypePriority = {
            {PlaybackType::Meditation, 1},
            {PlaybackType::CallRingtone, 2},
            {PlaybackType::Alarm, 3},
            {PlaybackType::Multimedia, 4},
            {PlaybackType::TextMessageRingtone, 5},
            {PlaybackType::Notifications, 5},
            {PlaybackType::KeypadSound, 6},
            {PlaybackType::None, static_cast<uint8_t>(PlaybackType::Last)},
        };
    } // namespace

    AudioMux::AudioMux(AudioServiceMessage::Callback callback, size_t audioInputsCount)
        : audioInputs(audioInputsInternal)
    {
        audioInputsCount = audioInputsCount > 0 ? audioInputsCount : 1;
        audioInputsInternal.reserve(audioInputsCount);
        for (size_t i = 0; i < audioInputsCount; i++) {
            audioInputsInternal.emplace_back(Input(std::make_unique<Audio>(callback), refToken.IncrementToken()));
        }
    }

    std::optional<AudioMux::Input *> AudioMux::GetRoutingInput(bool force)
    {
        if (auto input = GetInput({Audio::State::Routing})) {
            return input;
        }

        if (force) {
            auto *lowInput = &audioInputs.front();
            for (auto &audioInput : audioInputs) {
                auto lowestPrio  = GetPlaybackPriority(lowInput->audio->GetCurrentOperationPlaybackType());
                auto currentPrio = GetPlaybackPriority(audioInput.audio->GetCurrentOperationPlaybackType());

                if (currentPrio > lowestPrio) {
                    lowInput = &audioInput;
                }
            }
            LOG_DEBUG("Routing took over audio input.");
            return lowInput;
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetPlaybackInput(const audio::PlaybackType &playbackType)
    {
        // if routing or recording we cannot continue
        if (GetInput({Audio::State::Routing, Audio::State::Recording})) {
            return std::nullopt;
        }
        // try get with priority
        if (auto input = GetAvailableInput(playbackType)) {
            return input;
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetIdleInput()
    {
        return GetInput({Audio::State::Idle});
    }

    std::optional<AudioMux::Input *> AudioMux::GetActiveInput()
    {
        // first return active routing inputs
        if (auto routingInput = GetInput({Audio::State::Routing}); routingInput) {
            return routingInput;
        }
        for (auto &audioInput : audioInputs) {
            if (audioInput.audio->GetCurrentState() != Audio::State::Idle) {
                return &audioInput;
            }
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetInput(const std::vector<Audio::State> &states)
    {
        for (auto &audioInput : audioInputs) {
            if (std::find(states.begin(), states.end(), audioInput.audio->GetCurrentState()) != std::end(states)) {
                return &audioInput;
            }
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetInput(const Token &token)
    {
        if (!token.IsValid()) {
            return std::nullopt;
        }

        for (auto &audioInput : audioInputs) {
            // if has token - match or reject
            if (token == audioInput.token) {
                return &audioInput;
            }
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetAvailableInput(const audio::PlaybackType &playbackType)
    {
        std::optional<AudioMux::Input *> idleInput;
        std::optional<AudioMux::Input *> overridableInput;

        for (auto &audioInput : audioInputs) {
            auto currentPlaybackType = audioInput.audio->GetCurrentOperationPlaybackType();
            auto currentInputPrior   = GetPlaybackPriority(currentPlaybackType);

            // check busy input
            if (audioInput.audio->GetCurrentState() != Audio::State::Idle) {
                // handle priorities
                if (GetPlaybackPriority(playbackType) > currentInputPrior) {
                    return std::nullopt;
                }
                else if (GetPlaybackPriority(playbackType) <= currentInputPrior) {
                    if (currentPlaybackType == playbackType && IsMergable(currentPlaybackType)) {
                        // merge the sound if needed
                        overridableInput = std::nullopt;
                        break;
                    }
                    overridableInput = &audioInput;
                }
            }
            else {
                idleInput = &audioInput;
            }
        }

        return idleInput ? idleInput : overridableInput;
    }

    const Token AudioMux::ResetInput(std::optional<AudioMux::Input *> input)
    {
        if (input) {
            (*input)->DisableVibration();
            return (*input)->token = refToken.IncrementToken();
        }
        return refToken.IncrementToken();
    }

    uint8_t AudioMux::GetPlaybackPriority(const std::optional<audio::PlaybackType> type)
    {
        const auto &pmap = audio::PlaybackTypePriority;
        if (type && pmap.find(*type) != pmap.end()) {
            return pmap.at(*type);
        }
        return static_cast<uint8_t>(PlaybackType::Last);
    }

    constexpr bool AudioMux::IsMergable(const audio::PlaybackType &type) const
    {
        switch (type) {
        case PlaybackType::None:
            [[fallthrough]];
        case PlaybackType::Notifications:
            [[fallthrough]];
        case PlaybackType::KeypadSound:
            [[fallthrough]];
        case PlaybackType::TextMessageRingtone:
            return true;
        case PlaybackType::CallRingtone:
            [[fallthrough]];
        case PlaybackType::Meditation:
            [[fallthrough]];
        case PlaybackType::Alarm:
            [[fallthrough]];
        case PlaybackType::Multimedia:
            return false;
        }
        return false;
    }
} // namespace audio
