// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATParser.hpp"
#include <service-fota/FotaServiceAPI.hpp>
#include "bsp/cellular/bsp_cellular.hpp"
#include <service-cellular/CellularMessage.hpp>
#include "ticks.hpp"
#include <Utils.hpp>
#include <utility>
#include <vector>

ATParser::ATParser(bsp::Cellular *cellular) : cellular(cellular)
{
    isInitialized = true;
}

/// plz see 12.7 summary of urc in documentation
std::vector<ATParser::Urc> ATParser::ParseURC()
{

    std::vector<ATParser::Urc> resp;
    size_t maxPos = 0, pos = 0;

    cpp_freertos::LockGuard lock(mutex);

    std::vector<std::pair<std::string, ATParser::Urc>> vals = {
        {"RDY", ATParser::Urc::MeInitializationSuccessful},
        {"+CFUN: 1", ATParser::Urc::FullFuncionalityAvailable},
    };

    for (const auto &el : vals) {
        pos = responseBuffer.find(el.first);
        if (pos != std::string::npos) {
            resp.push_back(el.second);
            maxPos = std::max(pos + el.first.length(), maxPos);
            LOG_DEBUG("%s", ("[URC]: " + el.first).c_str());
        }
    }

    if (responseBuffer.find("+QIND: \"FOTA\"") != std::string::npos) {
        LOG_DEBUG("%s", responseBuffer.c_str());
        resp.push_back(ATParser::Urc::Fota);
        return resp;
    }

    // manage string buffer
    if (maxPos == 0) {}
    else if (responseBuffer.size() >= maxPos) {
        responseBuffer.erase();
    }
    else {
        responseBuffer = responseBuffer.substr(maxPos);
    }

    return resp;
}

int ATParser::ProcessNewData(sys::Service *service)
{
    char rawBuffer[256] = {0};

    LOG_DEBUG("Receiving data from ProcessNewData");
    auto length = cellular->Read(rawBuffer, sizeof(rawBuffer));

    {
        cpp_freertos::LockGuard lock(mutex);
        responseBuffer.append(reinterpret_cast<char *>(rawBuffer), length);
        LOG_DEBUG("Appending %d bytes to responseBuffer[%u]: %s",
                  static_cast<int>(length),
                  static_cast<unsigned int>(responseBuffer.size()),
                  utils::removeNewLines(responseBuffer).c_str());
    }

    auto ret = ParseURC();
    if (blockedTaskHandle) {
        xTaskNotifyGive(blockedTaskHandle);
    }
    else if (ret.size()) {
        if (ret.size() == 1 && ret[0] == ATParser::Urc::Fota) {
            std::string fotaData(responseBuffer);
            LOG_DEBUG("parsing FOTA:\"%s\"", fotaData.c_str());
            FotaService::API::sendRawProgress(service, fotaData);
            responseBuffer.erase();
        }
        else {
            urcs.insert(std::end(urcs), std::begin(ret), std::end(ret));
        }
        // GSM modem is considered as operational when it outputs URCs specified below:
        // 1) RDY
        // 2) +CFUN: 1
        if (urcs.size() == 2) {
            cpp_freertos::LockGuard lock(mutex);
            auto msg = std::make_shared<CellularPowerUpProcedureCompleteNotification>();
            service->bus.sendMulticast(msg, sys::BusChannel::ServiceCellularNotifications);
            LOG_DEBUG("[!!!] Fucking away data");
            responseBuffer.erase();
            urcs.clear();
        }
    }
    return 1;
}

void ATParser::cmd_init()
{
    cpp_freertos::LockGuard lock(mutex);
    responseBuffer.erase();
}

void ATParser::cmd_send(std::string cmd)
{
    cellular->Write(const_cast<char *>(cmd.c_str()), cmd.size());
}

std::string ATParser::cmd_receive()
{
    cpp_freertos::LockGuard lock(mutex);
    return responseBuffer;
}

void ATParser::cmd_post()
{
    cpp_freertos::LockGuard lock(mutex);
    responseBuffer.erase(); // TODO:M.P is it okay to flush buffer here ?
}
