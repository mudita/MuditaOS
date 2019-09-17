/*
 * Common.hpp
 *
 *  Created on: Mar 7, 2019
 *      Author: mati
 */

#ifndef SYSTEMMANAGER_COMMON_HPP_
#define SYSTEMMANAGER_COMMON_HPP_

#include "FreeRTOSConfig.h"

namespace sys
{

enum class BusChannels {
	System,
    SystemManagerRequests,
    PowerManagerRequests
#ifndef MODULE_CORE_CUSTOM_BUS

#else
,
#include "BusChannelsCustom.hpp"
#endif
};

enum class ServicePriority {
	Idle          	= 0,          ///< priority: idle (lowest)
	Low           	= 1,          ///< priority: low
	Normal   		= 2,          ///< priority: normal
	High          	= 3,          ///< priority: high
	Realtime     	= 4,          ///< priority: realtime (highest)
};

enum class ServicePowerMode{
    Active,
    SuspendToRAM,
    SuspendToNVM
};

enum class ReturnCodes{
    Success,
	Failure,
    Timeout,
    ServiceDoesntExist,
	//This is used in application's template in base class messages handler. The meaning is that
	//message that was processed by base class implementation of the DataReceivedHandler was not processed
	//and it should be handled by the class next in hierarchy.
	Unresolved
};

}



#endif /* SYSTEMMANAGER_COMMON_HPP_ */
