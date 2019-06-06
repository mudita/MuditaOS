
/*
 * @file ServiceDB.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_SERVICEDB_HPP
#define PUREPHONE_SERVICEDB_HPP


#include "Service/Service.hpp"
#include "Service/Message.hpp"

class ServiceDB: public sys::Service {
protected:

public:
    ServiceDB();
    ~ServiceDB();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

    static const char* serviceName;

};


#endif //PUREPHONE_SERVICEDB_HPP
