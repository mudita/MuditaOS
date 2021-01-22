// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

namespace bsp::battery_charger
{
	using StateOfCharge = std::uint8_t;

	enum class batteryRetval{
		OK,
		ChargerError,
		ChargerNotCharging,
		ChargerCharging
	};

	enum class batteryIRQSource{
		INTB = 0x01,
		INOKB = 0x02
	};

	enum class batteryINTBSource{
		minSOCAlert = 1 << 10,
		minVAlert = 1 << 8,
		SOCOnePercentChange = 1 << 7
	};

	int init(xQueueHandle queueHandle);

	void deinit();

	StateOfCharge getBatteryLevel();

	bool getChargeStatus();

	void clearAllIRQs();

	void clearFuelGuageIRQ();

	std::uint16_t getStatusRegister();

	BaseType_t INOKB_IRQHandler();

	BaseType_t INTB_IRQHandler();
} // bsp::battery_charger



