
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
#include "service-cellular/messages/CellularMessage.hpp"
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

    // Remove \r and \n for logging purposes
    std::string cmdStr(cmd);
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\r'), cmdStr.end());
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\n'), cmdStr.end());

    blockedTaskHandle = xTaskGetCurrentTaskHandle();
    cellular->Write((void*)cmd, strlen(cmd));

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
    uint32_t timeElapsed = currentTime;
    
    LOG_INFO("[AT]: %s, timeout value %d", cmdStr.c_str(), timeout);

    while(1) {

        if (timeElapsed >= timeoutNeeded)
        {
            LOG_MODEM_TIMEOUT("[AT]: %s, timeout %d - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf", cmdStr.c_str(), timeout);
            break;
        }

        auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
        timeElapsed = cpp_freertos::Ticks::GetTicks();
        if (ret)
        {
            std::vector<std::string> strings;

            cpp_freertos::LockGuard lock(mutex);
            // tokenize responseBuffer
            // empty lines are also removed
            auto ret = Tokenizer(responseBuffer, "\r\n", rxCount);
            tokens.insert(std::end(tokens), std::begin(ret), std::end(ret));

            if (tokens.size() < rxCount)
            {
                continue;
            }
        }
        else
        {
            LOG_MODEM_TIMEOUT("[AT]: %s, timeout %d - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf", cmdStr.c_str(), timeout);
        }
        
        break;
    }

    blockedTaskHandle = nullptr;
    responseBuffer.erase(); // TODO:M.P is it okay to flush buffer here ?
    LOG_INFO("[AT]: %s - returning %i tokens in %d ms", cmdStr.c_str(), tokens.size(), timeElapsed - currentTime);

#if DEBUG_MODEM_OUTPUT_RESPONSE
    for (auto s : tokens)
    {
        LOG_DEBUG("[]%s", s.c_str());
    }
#endif

    return tokens;
}

// TODO: alek: this tokenizer seems bad. Need to fix this
std::vector<std::string> ATParser::Tokenizer(const std::string &input, const std::string &delimiter, uint32_t maxTokenCount)
{
    std::vector<std::string> strings;
    uint32_t tokenCount = 0;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while (((pos = input.find(delimiter, prev)) != std::string::npos) && (maxTokenCount == 0 || tokenCount < maxTokenCount))
    {
        if (pos == prev) {
            prev = pos + delimiter.size();
            continue;
        }
        strings.push_back(input.substr(prev, pos - prev));
        prev = pos + delimiter.size();
        tokenCount++;
    }

    // To get the last substring (or only, if delimiter is not found)
    if (maxTokenCount == 0 || tokenCount < maxTokenCount)
    {
        auto str = input.substr(prev);
        if (!str.empty())
        {
            strings.push_back(str);
        }
    }
    return strings;
}