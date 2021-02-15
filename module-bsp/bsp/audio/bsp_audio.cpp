#include "bsp_audio.hpp"

#if defined(TARGET_RT1051)

#include "board/rt1051/bsp/audio/RT1051Audiocodec.hpp"
#include "board/rt1051/bsp/audio/RT1051CellularAudio.hpp"

#endif

#include <Audio/Stream.hpp>

#include <cassert>

namespace bsp
{

    std::optional<std::unique_ptr<AudioDevice>> AudioDevice::Create(bsp::AudioDevice::Type type)
    {
        std::unique_ptr<AudioDevice> inst;

        switch (type) {

        case Type ::Audiocodec: {
#if defined(TARGET_RT1051)
            inst = std::make_unique<bsp::RT1051Audiocodec>();
#else
            inst = nullptr;
#endif

        } break;

        case Type ::Bluetooth: {
            LOG_FATAL("Bluetooth audio is not yet supported");
            inst = nullptr;
        } break;

        case Type::Cellular: {
#if defined(TARGET_RT1051)
            inst = std::make_unique<bsp::RT1051CellularAudio>();
#else
            inst = nullptr;
#endif
        } break;
        }

        if (inst->isInitialized) {
            return inst;
        }

        return {};
    }
} // namespace bsp
