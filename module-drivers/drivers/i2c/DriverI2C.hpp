/*
 *  @file DriverI2C.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERI2C_HPP
#define PUREPHONE_DRIVERI2C_HPP

#include "../DriverInterface.hpp"

namespace drivers {


    enum class I2CInstances{
        I2C1
    };

    class DriverI2CParams {

    };


    class DriverI2C : public DriverInterface<DriverI2C> {
    public:

        static std::shared_ptr<DriverI2C> Create(const I2CInstances inst,const DriverI2CParams& params);

        DriverI2C(){}
        virtual ~DriverI2C(){}

        virtual ssize_t Write(const uint8_t *data, const size_t len, const DriverI2CParams &params) = 0;

        virtual ssize_t Read(uint8_t *data, const size_t len, const DriverI2CParams &params) = 0;

    };

}


#endif //PUREPHONE_DRIVERI2C_HPP
