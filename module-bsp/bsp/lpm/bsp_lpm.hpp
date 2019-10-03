/*
 *  @file bsp_lpm.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_BSP_LPM_HPP
#define PUREPHONE_BSP_LPM_HPP

#include <optional>
#include <memory>

namespace bsp {

    class LowPowerMode {
    public:

        enum class Mode{
            FullSpeed,
            LowPowerRun,
            LowPowerIdle,
            Suspend

        };

        static std::optional<std::unique_ptr<LowPowerMode>> Create();

        virtual int32_t Switch(const Mode mode) = 0;
        Mode GetCurrentMode(){return currentMode;}

        virtual int32_t PowerOff() = 0;


    protected:
        Mode currentMode = Mode::FullSpeed;

    };

}


#endif //PUREPHONE_BSP_LPM_HPP
