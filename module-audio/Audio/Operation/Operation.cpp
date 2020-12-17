// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Operation.hpp"

#include <algorithm>

#include "IdleOperation.hpp"
#include "PlaybackOperation.hpp"
#include "RecorderOperation.hpp"
#include "RouterOperation.hpp"

#include "Audio/decoder/Decoder.hpp"

#include <bsp/audio/bsp_audio.hpp>
#include <module-bsp/board/rt1051/bsp/audio/RT1051BluetoothAudio.hpp>

namespace audio
{
    std::unique_ptr<Operation> Operation::Create(
        Operation::Type t,
        const char *fileName,
        const audio::PlaybackType &playbackType,
        std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback)
    {
        std::unique_ptr<Operation> inst;

        switch (t) {
        case Type::Idle:
            inst = std::make_unique<IdleOperation>(fileName);
            break;
        case Type::Playback:
            inst = std::make_unique<PlaybackOperation>(fileName, playbackType, dbCallback);
            break;
        case Type::Router:
            inst = std::make_unique<RouterOperation>(fileName, dbCallback);
            break;
        case Type::Recorder:
            inst = std::make_unique<RecorderOperation>(fileName, dbCallback);
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

    void Operation::SetBluetoothStreamData(std::shared_ptr<BluetoothStreamData> data)
    {
        if (auto device = dynamic_cast<bsp::RT1051BluetoothAudio *>(audioDevice.get()); device != nullptr) {
            GetProfile()->SetSampleRate(data->metadata.sampleRate);
            device->sourceQueue = data->out;
            device->metadata    = data->metadata;
            LOG_INFO("Queue and metadata set!");
        }
    }

    audio::RetCode Operation::SwitchToPriorityProfile()
    {
        for (auto &p : supportedProfiles) {
            if (p.isAvailable == true) {
                return SwitchProfile(p.profile->GetType());
            }
        }
        return audio::RetCode::ProfileNotSet;
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
} // namespace audio
