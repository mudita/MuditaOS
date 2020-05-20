#include "common.hpp"

namespace bsp{
   	const char *c_str(const Board &board)
    {
            switch (board) {
            case Board::T3:
                return "T3";
                break;
            case Board::T4:
                return "T4";
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

