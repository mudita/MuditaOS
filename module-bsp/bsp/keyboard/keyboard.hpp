
/*
 * @file keyboard.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_KEYBOARD_HPP
#define PUREPHONE_KEYBOARD_HPP

#include <functional>
#include <stdint.h>


#include "common.hpp"


#include "module-sys/Service/Service.hpp"
#include "service-kbd/WorkerKbd.hpp"

#if defined(TARGET_RT1051)

#include "keyboard/key_codes_rt1051.hpp"

#elif defined(TARGET_Linux)
#include "keyboard/key_codes.hpp"
    //TODO:M.P insert Linux specific headers here
#else
#error "Unsupported target"
#endif


namespace bsp {

    class keyboard {
    public:

        RetCode Init(WorkerKbd* worker);
        RetCode DeInit();

    };

}


#endif //PUREPHONE_KEYBOARD_HPP
