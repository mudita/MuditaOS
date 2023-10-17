#include "common.hpp"

namespace bsp
{
    std::uint8_t CpuMHZToLevel(enum CpuFrequencyMHz val)
    {
        switch (val) {
        case CpuFrequencyMHz::Level_0:
           return 0;
        case CpuFrequencyMHz::Level_1:
           return 1;
        case CpuFrequencyMHz::Level_2:
           return 2;
        case CpuFrequencyMHz::Level_3:
           return 3;
        case CpuFrequencyMHz::Level_4:
           return 4;
        case CpuFrequencyMHz::Level_5:
           return 5;
        case CpuFrequencyMHz::Level_6:
           return 6;
        }
        return -1;
    }

   	const char *c_str(Board board)
    {
        switch (board) {
        case Board::RT1051:
            return "RT1051";
        case Board::Linux:
            return "Linux";
        default:
            return "none";
        }
   }
};
