//
// Created by mati on 09.09.2019.
//

#ifndef PUREPHONE_LINUXLPM_H
#define PUREPHONE_LINUXLPM_H

#include "bsp/lpm/bsp_lpm.hpp"

namespace bsp{

    class LinuxLPM : public LowPowerMode {
    public:
        int32_t Switch(const Mode mode) override final;
    private:
        Mode currentMode = Mode::FullSpeed;
    };

}




#endif //PUREPHONE_LINUXLPM_H
