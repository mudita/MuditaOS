
#include "AudioMux.hpp"
#include "Audio.hpp"

namespace audio
{
    AudioMux::AudioMux(audio::AsyncCallback asyncClbk, audio::DbCallback dbClbk, size_t audioInputsCount)
    {
        audioInputsCount = audioInputsCount > 0 ? audioInputsCount : 1;
        audioInputs.reserve(audioInputsCount);
        for (size_t i = 0; i < audioInputsCount; i++) {
            audioInputs.emplace_back(Input(Audio(asyncClbk, dbClbk), refToken.IncrementToken()));
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
                auto lowestPrio  = GetPlaybackPriority(lowInput->audio.GetCurrentOperation()->GetPlaybackType());
                auto currentPrio = GetPlaybackPriority(audioInput.audio.GetCurrentOperation()->GetPlaybackType());

                if (currentPrio > lowestPrio) {
                    lowInput = &audioInput;
                }
            }
            LOG_DEBUG("Routing took over audio input.");
            return lowInput;
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetRecordingInput()
    {
        if (auto input = GetInput({Audio::State::Recording, Audio::State::Routing})) {
            return input;
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetPlaybackInput(const Token &token,
                                                                const audio::PlaybackType &playbackType)
    {
        // if routing or recording we cannot continue
        if (GetInput({Audio::State::Routing, Audio::State::Recording})) {
            return std::nullopt;
        }
        // try get with token
        if (auto input = GetInput(token)) {
            return input;
        }
        // try get with priority
        if (auto input = GetInput(playbackType)) {
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
        for (auto &audioInput : audioInputs) {
            if (!audioInput.audio.GetCurrentOperation()) {
                continue;
            }
            if (audioInput.audio.GetCurrentState() != Audio::State::Idle &&
                audioInput.audio.GetCurrentOperation()->GetState() == audio::Operation::State::Active) {
                return &audioInput;
            }
        }
        return std::nullopt;
    }

    std::optional<AudioMux::Input *> AudioMux::GetInput(const std::vector<Audio::State> &states)
    {
        for (auto &audioInput : audioInputs) {
            if (std::find(states.begin(), states.end(), audioInput.audio.GetCurrentState()) != std::end(states)) {
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

    std::optional<AudioMux::Input *> AudioMux::GetInput(const audio::PlaybackType &playbackType)
    {
        std::optional<AudioMux::Input *> idleInput;
        std::optional<AudioMux::Input *> overridableInput;

        for (auto &audioInput : audioInputs) {
            auto currentPlaybackType = audioInput.audio.GetCurrentOperation()->GetPlaybackType();
            auto currentInputPrior   = GetPlaybackPriority(currentPlaybackType);

            // check busy input
            if (audioInput.audio.GetCurrentState() != Audio::State::Idle) {
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

    Token AudioMux::IncrementToken(std::optional<AudioMux::Input *> input)
    {
        if (input) {
            return (*input)->token = refToken.IncrementToken();
        }
        return Token::MakeBadToken();
    }

    uint8_t AudioMux::GetPlaybackPriority(const audio::PlaybackType &type) const
    {
        const auto &pmap = audio::PlaybackTypePriority;
        if (pmap.find(type) != pmap.end()) {
            return pmap.at(type);
        }
        return static_cast<uint8_t>(PlaybackType::Last);
    }

    bool AudioMux::IsMergable(const audio::PlaybackType &type) const
    {
        return !(type == audio::PlaybackType::Multimedia);
    }
} // namespace audio
