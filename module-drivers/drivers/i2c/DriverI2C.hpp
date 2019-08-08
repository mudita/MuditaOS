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

class DriverI2C : public DriverInterface {
public:

    static std::shared_ptr<DriverI2C> Create(const DriverParams &params);

    DriverI2C(const DriverParams &params);
    virtual ~DriverI2C();

    virtual ssize_t Write(const uint8_t *data, const size_t len, const DriverParams &params) = 0;

    virtual ssize_t Read(uint8_t *data, const size_t len, const DriverParams &params) = 0;

    virtual int32_t Ioctl(const IoctlParams &ioctlParams) = 0;

private:
    static std::weak_ptr<DriverI2C> instance;
};


#endif //PUREPHONE_DRIVERI2C_HPP
