/*
 * ServiceEink.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

//FreeRTOS
extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
}
//module-utils
#include "log/log.hpp"
//module-vfs
#include "vfs.hpp"
#include "EinkIncludes.hpp"

//eink messages
#include "messages/EinkMessage.hpp"
#include "messages/ImageMessage.hpp"
#include "service-gui/messages/GUIMessage.hpp"

#include "SystemManager/SystemManager.hpp"

#include "MessageType.hpp"

#include "ServiceEink.hpp"

enum class EinkWorkerCommands {
	Initialize,
	Initialized,
	Destroy,
	CopyImage,
	CopyCompleteCallback,
	CopyComplete
};


/// This is DMA handle for internal frame buffer memory-to-memory copying operation
//static edma_handle_t            s_einkMemcpyDma_handle;

/**
 * @brief This function is a callback for the memory to memory Eink HW copying completed event
 * @param
 */
static void s_EinkServiceDMAMemcpyCallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (isIRQ())
    {
        BaseType_t xhigherpriorityTaskToBeWokenUp = 0;
        xTaskNotifyFromISR((TaskHandle_t)param, 0, eNoAction, &xhigherpriorityTaskToBeWokenUp);
        portEND_SWITCHING_ISR( xhigherpriorityTaskToBeWokenUp );
    }
    else
    {
        xTaskNotify((TaskHandle_t)param, 0, eNoAction);
    }
}

ServiceEink::ServiceEink(const std::string& name)
	: sys::Service(name),
	  timerID { 0 },
	  selfRefereshTriggerCount{ 0 },
	  temperatureMeasurementTriggerCount{ 0 },
	  powerOffTriggerCount{ 0 }
{
	//initialize initial eink parameters
	memset(&waveformSettings, 0, sizeof(EinkWaveFormSettings_t));
	waveformSettings.mode = EinkWaveformGC16;
	waveformSettings.temperature = -1000;
}

ServiceEink::~ServiceEink(){
	LOG_INFO("[ServiceEink] Cleaning resources");
	//release data from settings
	if( waveformSettings.LUTCData != nullptr )
		delete [] waveformSettings.LUTCData;
	if( waveformSettings.LUTDData != nullptr )
		delete [] waveformSettings.LUTDData;
	//set sure that any new temperature will cause to load data from file.
	waveformSettings.temperature = -1000;

}

// Invoked upon receiving data message
sys::Message_t ServiceEink::DataReceivedHandler(sys::DataMessage* msgl) {
	seink::EinkMessage* msg = static_cast<seink::EinkMessage*>(msgl);

	switch( msg->messageType ) {

		case static_cast<uint32_t>(MessageType::MessageTypeUninitialized): {
			LOG_ERROR("[ServiceEink] Received uninitialized message type");
		} break;

		case static_cast<uint32_t>(MessageType::EinkImageData): {
			auto dmsg = static_cast<seink::ImageMessage*>( msgl );
			LOG_INFO("[ServiceEink] Received framebuffer");
			memcpy( einkRenderBuffer, dmsg->getData(), dmsg->getSize() );
			deepRefresh = dmsg->getDeepRefresh();
			auto msg = std::make_shared<sgui::GUIMessage>(MessageType::EinkDMATransfer );
			sys::Bus::SendUnicast(msg, this->GetName(), this);
		} break;
		case static_cast<uint32_t>(MessageType::EinkDMATransfer): {

			LOG_INFO("[ServiceEink] Received framebuffer");
			uint32_t start_tick = xTaskGetTickCount();
			EinkPowerOn();

			int32_t temperature = EinkGetTemperatureInternal();

			EinkStatus_e ret;
			//changeWaveform( EinkWaveforms_e::EinkWaveformDU2, temperature );
			if( deepRefresh ) {
				changeWaveform(EinkWaveforms_e::EinkWaveformGC16, temperature);
			}
			else{
				changeWaveform(EinkWaveforms_e::EinkWaveformDU2, temperature);
			}

			ret =
			EinkUpdateFrame ( 0,
			                  0,
			                  480,
			                  600,
							  einkRenderBuffer,
							  Eink4Bpp,
			                  EinkDisplayColorModeStandard );
			if( ret != EinkOK )
				LOG_FATAL("Failed to update frame");

			//changeWaveform( EinkWaveforms_e::EinkWaveformDU2, temperature );
			if( deepRefresh ) {
				ret = EinkRefreshImage (0, 0, 480, 600, EinkDisplayTimingsDeepCleanMode );
			}
			else{
				ret = EinkRefreshImage (0, 0, 480, 600, EinkDisplayTimingsFastRefreshMode );
			}

			if( ret != EinkOK )
				LOG_FATAL("Failed to refresh frame");
			EinkPowerOff();
			uint32_t end_tick = xTaskGetTickCount();

			LOG_INFO("[ServiceEink] RefreshTime: %d", end_tick - start_tick);

			auto msg = std::make_shared<sgui::GUIMessage>(MessageType::GUIDisplayReady );
			sys::Bus::SendUnicast(msg, "ServiceGUI", this);
		} break;

		case static_cast<uint32_t>(MessageType::EinkTemperatureUpdate): {

		} break;

		case static_cast<uint32_t>(MessageType::EinkStateRequest ): {
			auto msg = std::make_shared<sgui::GUIMessage>(MessageType::GUIDisplayReady );
			sys::Bus::SendUnicast(msg, "ServiceGUI", this);
		} break;

	};

	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceEink::TickHandler(uint32_t id) {

	LOG_INFO("[ServiceEink] Timer");
}

// Invoked during initialization
sys::ReturnCodes ServiceEink::InitHandler() {

	LOG_INFO("[ServiceEink] Initializing");

	EinkStatus_e einkStatus = EinkResetAndInitialize();

	if (einkStatus != EinkOK)
	{
		LOG_FATAL("Error: Could not initialize Eink display!\n");
	}

	EinkMemcpyDmaInit( s_EinkServiceDMAMemcpyCallback );

	//TODO remove screen clearing code below.
	EinkPowerOn();

	uint8_t s_einkAmbientTemperature = EinkGetTemperatureInternal();
	LOG_INFO("EInk measured temperature: %d\u00B0C", s_einkAmbientTemperature);

	// Make the saturation to the lower limit
	if (s_einkAmbientTemperature < 0)
		s_einkAmbientTemperature = 0;

	// Make the saturation to the higher limit
	if (s_einkAmbientTemperature > 49)
		s_einkAmbientTemperature = 49;

	// Clear the temperature timer count
	deepClearScreen( s_einkAmbientTemperature );
	EinkPowerOff();

	//TODO remove and add timer or turning off eink
	EinkPowerOn();

	auto msg = std::make_shared<sgui::GUIMessage>(MessageType::GUIDisplayReady );
	sys::Bus::SendUnicast(msg, "ServiceGUI", this);


	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceEink::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceEink::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceEink::SleepHandler() {
	return sys::ReturnCodes::Success;
}

bool ServiceEink::changeWaveform( EinkWaveforms_e mode, const int32_t temperature ) {

    // If neither the temperature nor the waveform has changed - do nothing
    if ((temperature == waveformSettings.temperature) &&
        (mode == waveformSettings.mode ))
    {
        return EinkOK;
    }

    const uint32_t LUTD_SIZE = 16385;
	const uint32_t LUTC_SIZE = 64;
	const uint32_t LUTR_SIZE = 256; ///< Needed due to \ref EINK_LUTS_FILE_PATH structure

	const uint32_t LUTS_TOTAL_SIZE = LUTD_SIZE + LUTC_SIZE + LUTR_SIZE;

	waveformSettings.temperature = temperature;

	unsigned int segment = 0;

	if (temperature < 38)
	{
		segment = temperature/3;
	}
	else
	{
		if (temperature < 43)
		{
			segment = 12;
		}
		else
		{
			segment = 13;
		}
	}

	uint32_t offset = 0;

	switch (mode)
	{
		case EinkWaveformINIT:
			offset = LUTS_TOTAL_SIZE*segment;
		break;

		case EinkWaveformA2:
			offset = LUTS_TOTAL_SIZE*(14+segment);
		break;

		case EinkWaveformDU2:
			offset = LUTS_TOTAL_SIZE*(28+segment);
		break;

		case EinkWaveformGLD16:
			offset = LUTS_TOTAL_SIZE*(42+segment);
		break;

		case EinkWaveformGC16:
		default:
			offset = LUTS_TOTAL_SIZE*(56+segment);
		break;
	}


    //Open file
	auto file = vfs.fopen( "sys/Luts.bin", "rb" );
    if ( file == nullptr )
    {
        LOG_FATAL("Could not find the LUTS.bin file. Returning");
        return false;
    }
    // Allocate memory for the LUTD data. +1 for the EinkLUTD command for SPI transfer
    if( waveformSettings.LUTDData != nullptr )
    	delete [] waveformSettings.LUTDData;

    waveformSettings.LUTDSize = 0;
    waveformSettings.LUTDData = new uint8_t[ LUTD_SIZE + 1];

    if(waveformSettings.LUTDData == nullptr )
    {
        LOG_ERROR("Could not allocate memory for the LUTD array");
        vfs.fclose( file );
        return false;
    }

    // Allocate memory for the LUTC data. +1 for the EinkLUTC command for SPI transfer
    if( waveformSettings.LUTCData != nullptr )
    	delete [] waveformSettings.LUTCData;

    waveformSettings.LUTCSize = LUTC_SIZE;
    waveformSettings.LUTCData = new uint8_t[ LUTC_SIZE + 1];
    if ( waveformSettings.LUTCData  == nullptr )
    {
        LOG_ERROR("Could not allocate memory for the LUTC array");
        vfs.fclose( file );
        return false;
    }

    waveformSettings.LUTDData[0] = EinkLUTD;
    waveformSettings.LUTCData[0] = EinkLUTC;

    ///LUTD
    vfs.fseek( file, offset, SEEK_SET );
    vfs.fread( &waveformSettings.LUTDData[1], 1, LUTD_SIZE, file );

    uint8_t frameCount = waveformSettings.LUTDData[1] + 1; // 0x00 - 1 frame, ... , 0x0F - 16 frames
    waveformSettings.LUTDSize = frameCount * 64 + 1 + 1; // (frameCount * 64) - size of actual LUT; (+1) - the byte containing frameCount; (+1) - EinkLUTD command

    //LUTC
	offset += LUTD_SIZE;
	vfs.fseek( file, offset, SEEK_SET );
	vfs.fread( &waveformSettings.LUTCData[1], 1, LUTC_SIZE, file );

	vfs.fclose( file );

	EinkUpdateWaveform( &waveformSettings );

	return true;
}

bool ServiceEink::deepClearScreen(int8_t temperature)
{
    EinkWaveforms_e wv = waveformSettings.mode;

//	changeWaveform( EinkWaveforms_e::EinkWaveformA2, temperature );
//    EinkFillScreenWithColor(EinkDisplayColorWhite);
//    EinkFillScreenWithColor(EinkDisplayColorBlack);
//    EinkFillScreenWithColor(EinkDisplayColorWhite);
//    EinkFillScreenWithColor(EinkDisplayColorBlack);
//    EinkFillScreenWithColor(EinkDisplayColorWhite);

    EinkPowerOn();
	changeWaveform( EinkWaveforms_e::EinkWaveformA2, temperature );

	EinkStatus_e ret;
	memset( einkRenderBuffer, 15, 480*600 );
	ret = EinkUpdateFrame ( 0,0, 480, 600, einkRenderBuffer, Eink4Bpp, EinkDisplayColorModeStandard );
	if( ret != EinkOK ) LOG_FATAL("Failed to update frame");
	ret = EinkRefreshImage (0, 0, 480, 600, EinkDisplayTimingsFastRefreshMode );
	if( ret != EinkOK ) LOG_FATAL("Failed to refresh frame");

	for( uint32_t i=0; i<2; i++) {
		memset( einkRenderBuffer, 0, 480*600 );
		ret = EinkUpdateFrame ( 0,0, 480, 600, einkRenderBuffer, Eink4Bpp, EinkDisplayColorModeStandard );
		if( ret != EinkOK ) LOG_FATAL("Failed to update frame");
		ret = EinkRefreshImage (0, 0, 480, 600, EinkDisplayTimingsFastRefreshMode );
		if( ret != EinkOK ) LOG_FATAL("Failed to refresh frame");

		memset( einkRenderBuffer, 15, 480*600 );
		ret = EinkUpdateFrame ( 0,0, 480, 600, einkRenderBuffer, Eink4Bpp, EinkDisplayColorModeStandard );
		if( ret != EinkOK ) LOG_FATAL("Failed to update frame");
		ret = EinkRefreshImage (0, 0, 480, 600, EinkDisplayTimingsFastRefreshMode );
		if( ret != EinkOK ) LOG_FATAL("Failed to refresh frame");
	}

	changeWaveform(wv, temperature);

	EinkPowerOff();

    return true;
}




