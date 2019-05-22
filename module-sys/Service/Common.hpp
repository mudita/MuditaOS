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
    SystemManagerRequests
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

enum class ReturnCodes{
    Success,
    Timeout,
    ServiceDoesntExist
};

}



#endif /* SYSTEMMANAGER_COMMON_HPP_ */
