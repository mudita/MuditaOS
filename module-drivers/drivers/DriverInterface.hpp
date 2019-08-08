/*
 *  @file DriverInterface.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERINTERFACE_HPP
#define PUREPHONE_DRIVERINTERFACE_HPP

#include <memory>

class DriverParams {

};

class IoctlParams {

};

class DriverInterface {

public:

    template<typename T>
    std::shared_ptr<DriverInterface> Create(T, const DriverParams &params){
        return T::Create(params);
    }

    DriverInterface(const DriverParams &params);
    virtual ~DriverInterface();

    virtual ssize_t Write(const uint8_t *data, const size_t len, const DriverParams &params) = 0;

    virtual ssize_t Read(uint8_t *data, const size_t len, const DriverParams &params) = 0;

    virtual int32_t Ioctl(const IoctlParams &ioctlParams) = 0;

};


#endif //PUREPHONE_DRIVERINTERFACE_HPP
