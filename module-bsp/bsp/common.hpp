
/*
 * @file common.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 20.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef MODULE_BSP_COMMON_HPP
#define MODULE_BSP_COMMON_HPP

//TODO maybe move KeyEvents to keyboard.hpp
namespace bsp
{
    enum class RetCode{
        Success,
        Failure

    };

    enum class KeyEvents{
        Released,
        Pressed,

    };


}

#endif //MODULE_BSP_COMMON_HPP
