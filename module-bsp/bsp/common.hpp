
/*
 * @file common.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 20.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#pragma once

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


    enum class Board{
    	T3,
    	T4,
    	Linux,
    	none
    };
    [[nodiscard]] const char *c_str(const Board &board);

}

