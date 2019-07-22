/*
 *  @file bsp_audio.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "bsp_audio.hpp"

#if defined(TARGET_RT1051)

#elif defined(TARGET_Linux)
#include "audio/linux_audiocodec.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp{

    std::optional<std::unique_ptr<AudioDevice>> AudioDevice::Create(bsp::AudioDevice::Type type,audioCallback_t callback) {

        std::unique_ptr<AudioDevice> inst;

        switch(type){

            case Type ::Audiocodec:
            {
#if defined(TARGET_RT1051)

#elif defined(TARGET_Linux)
                inst = std::make_unique<bsp::LinuxAudiocodec>(callback );
#else
                #error "Unsupported target"
#endif

            }
                break;


            case Type ::Bluetooth:
            {
#if defined(TARGET_RT1051)

#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif
            }
                break;

            case Type::Cellular:
            {
#if defined(TARGET_RT1051)

#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif
            }
                break;

        }

        if(inst->isInitialized){
            return inst;
        }

        return {};
    }

}