// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Operation.hpp"

#include <algorithm>

#include "Audio/AudioDevice.hpp"
#include "Audio/AudioDeviceFactory.hpp"

#include "IdleOperation.hpp"
#include "PlaybackOperation.hpp"
#include "RecorderOperation.hpp"
#include "RouterOperation.hpp"

namespace audio
{
    std::unique_ptr<Operation> Operation::Create(Operation::Type t,
                                                 const char *fileName,
                                                 const audio::PlaybackType &playbackType,
                                                 AudioServiceMessage::Callback callback)
    {
        std::unique_ptr<Operation> inst;

        switch (t) {
        case Type::Idle:
            inst = std::make_unique<IdleOperation>(fileName);
            break;
        case Type::Playback:
            inst = std::make_unique<PlaybackOperation>(fileName, playbackType, callback);
            break;
        case Type::Router:
            inst = std::make_unique<RouterOperation>(fileName, callback);
            break;
        case Type::Recorder:
            inst = std::make_unique<RecorderOperation>(fileName, callback);
            break;
        }

        inst->opType   = t;
        inst->filePath = fileName;
        return inst;
    }

    std::shared_ptr<Profile> Operation::GetProfile(const Profile::Type type)
    {
        auto ret = std::find_if(supportedProfiles.begin(), supportedProfiles.end(), [type](const auto &w) {
            return w.isAvailable == true && w.profile->GetType() == type;
        });
        if (ret == supportedProfiles.end()) {
            return nullptr;
        }
        else {
            return ret->profile;
        }
    }

    std::optional<Profile::Type> Operation::GetPriorityProfile() const
    {
        for (auto &p : supportedProfiles) {
            if (p.isAvailable == true) {
                return p.profile->GetType();
            }
        }
        return {};
    }

    audio::RetCode Operation::SwitchToPriorityProfile()
    {
        if (const auto priorityProfile = GetPriorityProfile(); priorityProfile.has_value()) {
            return SwitchProfile(priorityProfile.value());
        }
        return audio::RetCode::ProfileNotSet;
    }

    audio::RetCode Operation::SwitchToPriorityProfile([[maybe_unused]] audio::PlaybackType playbackType)
    {
        return SwitchToPriorityProfile();
    }

    void Operation::SetProfileAvailability(std::vector<Profile::Type> profiles, bool available)
    {
        for (auto &p : supportedProfiles) {
            if (auto shouldSet = (std::find(profiles.begin(), profiles.end(), p.profile->GetType()) != profiles.end());
                shouldSet) {
                p.isAvailable = available;
            }
        }
    }
    void Operation::AddProfile(const Profile::Type &profile, const PlaybackType &playback, bool isAvailable)
    {
        const auto reqVol  = AudioServiceMessage::DbRequest(Setting::Volume, playback, profile);
        const auto reqGain = AudioServiceMessage::DbRequest(Setting::Gain, playback, profile);

        std::optional<audio::Volume> volume;
        std::optional<audio::Gain> gain;

        if (auto val = serviceCallback(&reqVol); val) {
            volume = utils::getNumericValue<audio::Volume>(val.value());
        }

        if (auto val = serviceCallback(&reqGain); val) {
            gain = utils::getNumericValue<audio::Gain>(val.value());
        }

        supportedProfiles.emplace_back(Profile::Create(profile, volume, gain), isAvailable);
    }

    std::shared_ptr<AudioDevice> Operation::CreateDevice(const Profile &profile)
    {
        return factory->CreateDevice(profile);
    }

    std::shared_ptr<AudioDevice> Operation::createCellularAudioDevice()
    {
        return factory->createCellularAudioDevice();
    }
} // namespace audio
