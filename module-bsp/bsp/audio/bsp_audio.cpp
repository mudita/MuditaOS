#include "bsp_audio.hpp"

#if defined(TARGET_RT1051)

#include "board/rt1051/bsp/audio/RT1051Audiocodec.hpp"
#include "board/rt1051/bsp/audio/RT1051CellularAudio.hpp"

#include <Audio/Stream.hpp>

#include <cassert>

#elif defined(TARGET_Linux)
#include "audio/linux_audiocodec.hpp"
#include "audio/LinuxCellularAudio.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp
{

    std::optional<std::unique_ptr<AudioDevice>> AudioDevice::Create(bsp::AudioDevice::Type type,
                                                                    audioCallback_t callback)
    {
        std::unique_ptr<AudioDevice> inst;

        switch (type) {

        case Type ::Audiocodec: {
#if defined(TARGET_RT1051)
            inst = std::make_unique<bsp::RT1051Audiocodec>(callback);
#elif defined(TARGET_Linux)
            inst = std::make_unique<bsp::LinuxAudiocodec>(callback);
#else
#error "Unsupported target"
#endif

        } break;

        case Type ::Bluetooth: {
#if defined(TARGET_RT1051)
            inst = nullptr;

#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif
        } break;

        case Type::Cellular: {
#if defined(TARGET_RT1051)
            inst = std::make_unique<bsp::RT1051CellularAudio>(callback);
#elif defined(TARGET_Linux)
            inst = std::make_unique<bsp::LinuxCellularAudio>(callback);
#else
#error "Unsupported target"
#endif
        } break;
        }

        if (inst->isInitialized) {
            return inst;
        }

        return {};
    }
} // namespace bsp
