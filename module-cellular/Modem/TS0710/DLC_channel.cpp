/**
 * Project Untitled
 */


#include "DLC_channel.h"
#include "TS0710_DLC_RELEASE.h"
#include "TS0710_DATA.h"
#include "TS0710_DLC_ESTABL.h"
#include "TS0710.h"
#include "log/log.hpp"
#include "TS0710_Frame.h"
#include "ticks.hpp"
#include <cstdlib>
#include "Modem/ATParser.hpp"

/**
 * DLC_channel implementation
 */

DLC_channel::DLC_channel(DLCI_t DLCI, std::string name, bsp::Cellular *cellular, Callback_t callback){
    LOG_DEBUG("Creating DLCI %i channel \"%s\"", DLCI, name.c_str());
    pv_name = name;
    pv_DLCI = DLCI;
    pv_cellular = cellular;

    if (callback != nullptr)
        pv_callback = callback;

    DLC_ESTABL_SystemParameters_t system_parameters;
    system_parameters.TypeOfFrame = TypeOfFrame_e::SABM;
    system_parameters.ConvergenceLayer = 1;
    system_parameters.Priority = 1;
    system_parameters.AckTime = 100; //100ms default
    system_parameters.MaxFrameSize = 128;
    system_parameters.MaxNumOfRetransmissions = 3; //default 3
    system_parameters.ErrRecovWindowSize = 2;      //default 2

    TS0710_DLC_ESTABL establ = TS0710_DLC_ESTABL(DLCI, system_parameters, cellular);
    pv_chanParams = establ.getParams();

    //wait for return & set active
    active = establ.getResponse();
    LOG_DEBUG("Create channel %s: %s", pv_name.c_str(), active ? "TRUE" : "FALSE");
}

DLC_channel::~DLC_channel(){
    TS0710_DLC_RELEASE release = TS0710_DLC_RELEASE(pv_DLCI);
}

void DLC_channel::SendData(std::vector<uint8_t> & data){
    TS0710_DATA _data = TS0710_DATA(pv_DLCI, pv_chanParams, data, pv_cellular);
}

#if 0  //left here for reference
ssize_t DLC_channel::ReceiveData(std::vector<uint8_t> &data, uint32_t timeout) {
    ssize_t ret = -1;
    static uint8_t *buf = nullptr;
    buf = reinterpret_cast<uint8_t*>(malloc(pv_chanParams.MaxFrameSize));
    bool complete = false;

    while((!complete) && (timeout--)) {  //TODO: add timeout control
        ret = pv_cellular->Read(reinterpret_cast<void *>(buf), pv_chanParams.MaxFrameSize);
        if (ret > 0) {
            LOG_DEBUG("Received %i bytes", ret);
            for (int i = 0; i < ret; i++)
                data.push_back(buf[i]);
            complete = TS0710_Frame::isComplete(data);
        }
        sleep_ms(1);
    }
    if (!complete)
        LOG_ERROR("Incomplete frame received");
    if (timeout == 0)
        LOG_ERROR("Timeout occured");
    
    free(buf);

    if (!TS0710_Frame::isMyChannel(data, pv_DLCI)) {
        data.clear();
        ret = -1;
    }

    return ret;
}
#endif

inline void LogTimeout(const std::string &cmdStr, uint32_t timeout)
{
    LOG_MODEM_TIMEOUT("[AT]: %s, timeout %d - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf", cmdStr.c_str(), timeout);
}

inline void LogDebugModemOutputResponse(const std::string &cmdStr, const std::vector<std::string> &tokens, const uint32_t timeElapsed,
                                        const uint32_t currentTime)
{
#if DEBUG_MODEM_OUTPUT_RESPONSE
    LOG_INFO("[AT]: %s - returning %i tokens in %d ms", cmdStr.c_str(), tokens.size(), timeElapsed - currentTime);

    for (auto s : tokens)
    {
        LOG_DEBUG("[]%s", s.c_str());
    }
#endif
}

std::vector<std::string> DLC_channel::SendCommandResponse(const char *cmd,
		size_t rxCount, uint32_t timeout) {

	std::vector<std::string> tokens;
	std::vector<uint8_t> data(cmd, cmd + strlen(cmd));

    // Remove \r and \n for logging purposes
    std::string cmdStr(cmd);
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\r'), cmdStr.end());
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\n'), cmdStr.end());

    LOG_INFO("[AT]: %s, timeout value %d", cmdStr.c_str(), timeout);

	blockedTaskHandle = xTaskGetCurrentTaskHandle();
	SendData(data);


	uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
	uint32_t timeoutNeeded =
			timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
	uint32_t timeElapsed = currentTime;

	//wait_for_data:
	while (1) {

        if (timeElapsed >= timeoutNeeded)
        {
            LogTimeout(cmdStr, timeout);
            break;
        }

		auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
		timeElapsed = cpp_freertos::Ticks::GetTicks();
		if (ret) {


			std::vector<std::string> strings;

			cpp_freertos::LockGuard lock(mutex);
			TS0710_Frame::frame_t frame;
			std::vector<uint8_t> v(responseBuffer.begin(),
					responseBuffer.end());

			responseBuffer.clear();
			std::vector<std::vector<uint8_t>> mFrames;
			std::vector<uint8_t> rawBuffer;

			//get frames from buffer
			for (int i = 0; i < v.size(); i++) {
				rawBuffer.push_back(v[i]);
				if (/*TS0710_Frame::isComplete(rawBuffer)*/(rawBuffer.size() > 1)
						&& (rawBuffer[0] == 0xF9) && (rawBuffer[rawBuffer.size() - 1] == 0xF9)) {
					//LOGrawBufferEBUG("Pushing back FRAME");
					mFrames.push_back(rawBuffer);
					rawBuffer.clear();
				}
			}

			//deseriaise data from received frames
			std::string deserialisedData;
			for (std::vector<uint8_t> vv : mFrames) {
				frame.deserialize(vv);
				std::string str(frame.data.begin(), frame.data.end());
				//append deserialised buffer
				deserialisedData += str;
			}
			mFrames.clear();

			//tokenize data
			LOG_DEBUG("[Tokenizing] frame");
			auto ret = ATParser::Tokenizer(deserialisedData, rxCount, "\r\n");
			tokens.insert(std::end(tokens), std::begin(ret), std::end(ret));
			if (tokens.size() < rxCount) {
				continue;
			}
		} else {
			//timeout
            LogTimeout(cmdStr, timeout);
        }
        break;
    }

    LogDebugModemOutputResponse(cmdStr, tokens, timeElapsed, currentTime);

    blockedTaskHandle = nullptr;

    return tokens;
}

std::vector<std::string> DLC_channel::SendCommandPrompt(const char *cmd,
		size_t rxCount, uint32_t timeout) {
	std::vector<std::string> tokens;
	std::vector<uint8_t> data(cmd, cmd + strlen(cmd));

    // Remove \r and \n for logging purposes
    std::string cmdStr(cmd);
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\r'), cmdStr.end());
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\n'), cmdStr.end());

    LOG_INFO("[AT]: %s, timeout value %d", cmdStr.c_str(), timeout);

	blockedTaskHandle = xTaskGetCurrentTaskHandle();
	SendData(data);


	uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
	uint32_t timeoutNeeded =
			timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
	uint32_t timeElapsed = currentTime;

	//wait_for_data:
	while (1) {

		if (timeElapsed >= timeoutNeeded)
		{
            LogTimeout(cmdStr, timeout);
            break;
		}

		auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
		timeElapsed = cpp_freertos::Ticks::GetTicks();
		if (ret) {

			std::vector<std::string> strings;

			cpp_freertos::LockGuard lock(mutex);
			TS0710_Frame::frame_t frame;
			std::vector<uint8_t> v(responseBuffer.begin(),
					responseBuffer.end());
			responseBuffer.clear();
			frame.deserialize(v);
			std::string str(frame.data.begin(), frame.data.end());
			//tokenize responseBuffer
			auto pos = str.find(">");
			if (pos != std::string::npos) {
				tokens.push_back(str.substr(pos, strlen(">")));
			}
			if (tokens.size() < rxCount) {
				continue;
			}
		} else {
			//timeout
            LogTimeout(cmdStr, timeout);
        }
        break;
    }

    LogDebugModemOutputResponse(cmdStr, tokens, timeElapsed, currentTime);

    blockedTaskHandle = nullptr;

    return tokens;
}

int DLC_channel::ParseInputData(std::vector<uint8_t> &data) {

    cpp_freertos::LockGuard lock(mutex);

    if(blockedTaskHandle){
        responseBuffer.append(reinterpret_cast<char *>(data.data()), data.size());
        xTaskNotifyGive(blockedTaskHandle);
    }
    else if (pv_callback != nullptr) {
            std::vector<uint8_t> v;
            for (auto c : data)
                v.push_back(c);
            pv_callback(v);
    }
    // else{
    //     LOG_DEBUG("Dropping unneeded frame");
    // }

    return 1;
}
