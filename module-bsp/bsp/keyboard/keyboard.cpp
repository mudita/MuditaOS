
/*
 * @file keyboard.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "keyboard.hpp"


#if defined(TARGET_RT1051)

#include "keyboard/rt1051_keyboard.hpp"
#elif defined(TARGET_Linux)
    //TODO:M.P insert Linux keyboard implementation here

#include "keyboard/linux_keyboard.hpp"

#else
#error "Unsupported target"
#endif

namespace bsp
{
    RetCode keyboard::Init(WorkerKbd* worker)
    {
#if defined(TARGET_RT1051)
        rt1501_keyboard_Init(signal, s);

#elif defined(TARGET_Linux)
        linux_keyboard_Init(worker);
#else
#error "Unsupported target"
#endif
        // TODO:M.P check return codes
        return RetCode ::Success;

    }

    RetCode keyboard::DeInit()
    {
#if defined(TARGET_RT1051)
        rt1501_keyboard_Deinit();
#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif

        // TODO:M.P check return codes
        return RetCode ::Success;
    }
}

