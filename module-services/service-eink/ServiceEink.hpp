/*
 * ServiceGUI.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_EINK_SERVICEEINK_HPP_
#define MODULE_SERVICES_SERVICE_EINK_SERVICEEINK_HPP_

//#include "board.h"

#include "Service/Service.hpp"
#include "Service/Message.hpp"
//eink bsp
#include "EinkIncludes.hpp"

class ServiceEink: public sys::Service {
protected:
	//this is timer that triggers 3 handlers - self refresh, temperature measurement and power off
	uint32_t timerID = 0;

	//counts timer triggers from last self refresh
	uint32_t selfRefereshTriggerCount;
	//counts timer events from last temperature measurement
	uint32_t temperatureMeasurementTriggerCount;
	//counts trigger counts from last action that required eink to be powered on
	uint32_t powerOffTriggerCount;

	//number of timer triggers required to execute self refresh handler
	const uint32_t selfRefereshTriggerValue = 60;
	//number of timer triggers required to execute temperature measurement handler
	const uint32_t temperatureMeasurementTriggerValue = 5*60;
	//number of timer triggers from last action requiring power on eink to power down eink.
	const uint32_t powerOffTriggerValue = 3;

	//structure with recently loaded waveformdata
	EinkWaveFormSettings_t waveformSettings;

	bool suspended = false;

	bool suspendInProgress = false;
	bool shutdownInProgress = false;

	bool changeWaveform( EinkWaveforms_e Mode, const int32_t temperature );

	bool deepClearScreen(int8_t temperature);

	uint8_t einkRenderBuffer[600 * 480];
	bool	deepRefresh = false;
	uint32_t timerPowerOffID = 0;
public:
	ServiceEink(const std::string& name,std::string parent = "");
    ~ServiceEink();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;
};

#endif /* MODULE_SERVICES_SERVICE_EINK_SERVICEEINK_HPP_ */
