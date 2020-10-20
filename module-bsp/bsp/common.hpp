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

