
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

#if defined(TARGET_RT1051)

#include "keyboard/key_codes_rt1051.hpp"
#else
#error "Unsupported target"
#endif


#include "common.hpp"

namespace bsp {

    class keyboard {

#if defined(TARGET_RT1051)

#include "keyboard/key_codes_rt1051.hpp"
#else
#error "Unsupported target"
#endif

        enum class KeyEvents{
            Pressed,
            Released
        };

        RetCode Init();
        RetCode DeInit();
        RetCode RegisterEventSignal(std::function<RetCode(KeyEvents event,KeyCodes code)> init);

    };

}


#endif //PUREPHONE_KEYBOARD_HPP
