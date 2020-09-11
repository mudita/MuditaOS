#include "Operation.hpp"

#include <algorithm>

#include "IdleOperation.hpp"
#include "PlaybackOperation.hpp"
#include "RecorderOperation.hpp"
#include "RouterOperation.hpp"

#include "bsp/audio/bsp_audio.hpp"
#include "Audio/decoder/decoder.hpp"

namespace audio
{
    std::optional<std::unique_ptr<Operation>> Operation::Create(
        Operation::Type t,
        const char *fileName,
        std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback)
    {
        std::unique_ptr<Operation> inst;

        switch (t) {
        case Type::Idle:
            inst = std::make_unique<IdleOperation>(fileName);
            break;
        case Type::Playback:
            inst = std::make_unique<PlaybackOperation>(fileName, dbCallback);
            break;
        case Type::Router:
            inst = std::make_unique<RouterOperation>(fileName, dbCallback);
            break;
        case Type::Recorder:
            inst = std::make_unique<RecorderOperation>(fileName, dbCallback);
            break;
        }

        if (inst->isInitialized) {
            return inst;
        }
        else {
            return {};
        }
    }

    std::optional<Profile *> Operation::GetProfile(const Profile::Type type)
    {
        auto ret = std::find_if(
            availableProfiles.begin(), availableProfiles.end(), [type](const auto &w) { return w->GetType() == type; });
        if (ret == availableProfiles.end()) {
            return {};
        }
        else {
            return (*ret).get();
        }
    }

} // namespace audio