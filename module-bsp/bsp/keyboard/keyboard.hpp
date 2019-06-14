
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



#include "service-kbd/WorkerEvent.hpp"

#include "key_codes.hpp"

namespace bsp {

    class keyboard {
    public:

        RetCode Init(WorkerEvent* worker);
        RetCode DeInit();

    };

}


#endif //PUREPHONE_KEYBOARD_HPP
