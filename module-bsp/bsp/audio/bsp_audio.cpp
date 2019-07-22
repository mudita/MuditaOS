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

#else
#error "Unsupported target"
#endif

namespace bsp{

    std::optional<std::unique_ptr<Audio>> Audio::Create(bsp::Audio::Type type) {

        std::unique_ptr<Audio> inst;

        switch(type){

            case Type ::Audiocodec:
            {

            }
                break;


            case Type ::Bluetooth:
            {

            }
                break;

            case Type::Cellular:
            {

            }
                break;

        }

        if(inst->isInitialized){
            return inst;
        }

        return {};
    }

}