
/*
 * @file ATParser.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ATParser.hpp"
#include "bsp/cellular/bsp_cellular.hpp"
#include "ticks.hpp"
#include "../../module-services/service-cellular/messages/CellularMessage.hpp"
#include "Service/Bus.hpp"

ATParser::ATParser(bsp::Cellular *cellular) : cellular(cellular) {
    isInitialized = true;
}

std::vector<ATParser::Urc> ATParser::ParseURC() {

    std::vector<ATParser::Urc> resp;
    size_t maxPos = 0;

    cpp_freertos::LockGuard lock(mutex);

    auto pos = responseBuffer.find("RDY");
    if (pos != std::string::npos) {
        resp.push_back(ATParser::Urc::MeInitializationSuccessful);
        maxPos = std::max(pos + strlen("RDY"), maxPos);
        LOG_DEBUG("Received URC: MeInitializationSuccessful");
    }

    pos = responseBuffer.find("+CFUN: 1");
    if (pos != std::string::npos) {
        resp.push_back(ATParser::Urc::FullFuncionalityAvailable);
        maxPos = std::max(pos + strlen("+CFUN: 1"), maxPos);
        LOG_DEBUG("Received URC: FullFuncionalityAvailable");
    }
    pos = responseBuffer.find("+CPIN: READY");
    if (pos != std::string::npos) {
        resp.push_back(ATParser::Urc::SimCardReady);
        maxPos = std::max(pos + strlen("+CPIN: READY"), maxPos);
        LOG_DEBUG("Received URC: SimCardReady");
    }

    pos = responseBuffer.find("+QIND: SMS DONE");
    if (pos != std::string::npos) {
        resp.push_back(ATParser::Urc::SMSInitializationComplete);
        maxPos = std::max(pos + strlen("+QIND: SMS DONE"), maxPos);
        LOG_DEBUG("Received URC: SMSInitializationComplete");
    }

    pos = responseBuffer.find("+QIND: PB DONE");
    if (pos != std::string::npos) {
        resp.push_back(ATParser::Urc::PhonebookInitializationComplete);
        maxPos = std::max(pos + strlen("+QIND: PB DONE"), maxPos);
        LOG_DEBUG("Received URC: PhonebookInitializationComplete");
    }

    // manage string buffer
    if(maxPos == 0){

    }
    else if (responseBuffer.size() >= maxPos) {
        responseBuffer.erase();
    } else {
        responseBuffer = responseBuffer.substr(maxPos);
    }

    return resp;
}

int ATParser::ProcessNewData(sys::Service *service) {
    char rawBuffer[256] = {0};

    LOG_DEBUG("Receiving data from ProcessNewData");
    auto length = cellular->Read(rawBuffer, sizeof(rawBuffer));

    {
        cpp_freertos::LockGuard lock(mutex);
        LOG_DEBUG("Appending %i bytes to responseBuffer", length);
        responseBuffer.append(reinterpret_cast<char *>(rawBuffer), length);
    }

    if (blockedTaskHandle) {
        xTaskNotifyGive(blockedTaskHandle);
    } else {
        auto ret = ParseURC();
        urcs.insert(std::end(urcs),std::begin(ret),std::end(ret));
        // GSM modem is considered as fully operational when it outputs URCs specified below:
        // 1) RDY
        // 2) +CFUN: 1
        // 3) +CPIN: READY
        // 4) +QIND: SMS DONE
        // 5) +QIND: PB DONE
        if (urcs.size() == 5) {
            cpp_freertos::LockGuard lock(mutex);
            //mux->callback(NotificationType ::PowerUpProcedureComplete,"");
            auto msg = std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::PowerUpProcedureComplete);
            sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, service);
            LOG_DEBUG("[!!!] Fucking away data");
            responseBuffer.erase();
            urcs.clear();
        }

    }
    return 1;
}

std::vector<std::string> ATParser::SendCommand(const char *cmd, size_t rxCount, uint32_t timeout) {
    std::vector<std::string> tokens;

    {
        cpp_freertos::LockGuard lock(mutex);
        responseBuffer.erase();
    }

    LOG_DEBUG("[AT]: %s", cmd);
    blockedTaskHandle = xTaskGetCurrentTaskHandle();
    //auto cmdSigned = const_cast<char *>(cmd);
    //inOutSerialWorker->WriteData(reinterpret_cast<unsigned char *>(cmdSigned), strlen(cmd));
    cellular->Write((void*)cmd, strlen(cmd));

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
    uint32_t timeElapsed = currentTime;

    wait_for_data:

    if (timeElapsed >= timeoutNeeded) {
        blockedTaskHandle = nullptr;
        return tokens;
    }

    auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
    timeElapsed = cpp_freertos::Ticks::GetTicks();
    if (ret) {

        std::vector<std::string> strings;

        cpp_freertos::LockGuard lock(mutex);
        //tokenize responseBuffer
        auto ret = Tokenizer(responseBuffer, rxCount, "\r\n");
        tokens.insert(std::end(tokens), std::begin(ret), std::end(ret));

        if (tokens.size() < rxCount) {
            goto wait_for_data;
        }

    }

    blockedTaskHandle = nullptr;
    responseBuffer.erase(); // TODO:M.P is it okay to flush buffer here ?
    LOG_DEBUG("AT command returning %i tokens", tokens.size());
    return tokens;
}

std::vector<std::string> ATParser::Tokenizer(std::string &input, uint32_t maxTokenCount,
                                             const std::string &delimiter) {
    std::vector<std::string> strings;
    uint32_t tokenCount = 0;


    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while (((pos = input.find(delimiter, prev)) != std::string::npos) && (tokenCount < maxTokenCount)) {
        if (pos == prev) {
            prev = pos + delimiter.size();
            continue;
        }

        strings.push_back(input.substr(prev, pos - prev));
        prev = pos + delimiter.size();
        tokenCount++;
    }

    // To get the last substring (or only, if delimiter is not found)
    input = input.substr(prev);
    return strings;
}
