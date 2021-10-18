#include "common.hpp"

namespace bsp{
   	const char *c_str(const Board &board)
    {
            switch (board) {
            case Board::RT1051:
                return "RT1051";
                break;
            case Board::Linux:
                return "Linux";
                break;
            case Board::none:
            default:
                return "none";
                break;
        }
   }
};

