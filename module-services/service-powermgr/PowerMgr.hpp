
/*
 * @file PowerMgr.hpp
 * @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 * @date 08.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_POWERMGR_HPP
#define PUREPHONE_POWERMGR_HPP

#include "Service/Bus.hpp"
#include "Service/Service.hpp"
#include "MessageType.hpp"
#include "Application.hpp"
#include "SystemManager/SystemManager.hpp"
//#include "messages/APMMessage.hpp"
#include "i18/i18.hpp"
extern "C" {
#include "lpm.h"
}

class PMMessage: public sys::DataMessage {
public:
    PMMessage( MessageType messageType ) : sys::DataMessage(static_cast<uint32_t>(messageType)),type(messageType) {};
    virtual ~PMMessage() {};

    MessageType type;

    uint32_t data;
};

class PowerMgr: public sys::Service {
private:
    uint32_t timer_id= 0;
    uint32_t pmode = 0;

protected:

public:
    PowerMgr();
    ~PowerMgr();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

    static const char* serviceName;

    /**
     * @brief Sends request to application manager to switch from current application to specific window in application with specified name .
     */
    static bool messageSwitchApplication( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<gui::SwitchData> data );
    /**
     * @brief Sends request to application manager to switch from current application to specific window in application with specified name.
     * Allows sending data to destination application.
     */
    //    static bool messageSwitchApplicationWithData( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<app::SwitchData>& switchData );
    /**
     * @Brief Function allows sending confirmation to the switch request. This is sent both by application that gains and looses focus.
     */
    static bool messageConfirmSwitch( sys::Service* sender);
    /**
     * @brief Function allows application to confirm close request.
     */
    static bool messageConfirmClose( sys::Service* sender);
    /**
     * @brief Allows requesting Application Manager to run previous application.
     */
    static bool messageSwitchPreviousApplication( sys::Service* sender );
    /**
     * @brief Sends information from application to manager about result of application's init function.
     * If successful message will contain name and true value, otherwise false value will be transmitted.
     */
    static bool messageRegisterApplication( sys::Service* sender, const bool& status );

    void APP_PrintRunFrequency(int32_t run_freq_only);

    void SetPowerMode(lpm_power_mode_t mode);
};

#endif //PUREPHONE_POWERMGR_HPP
