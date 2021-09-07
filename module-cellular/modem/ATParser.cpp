// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATParser.hpp"
#include <service-fota/FotaServiceAPI.hpp>
#include "bsp/cellular/bsp_cellular.hpp"
#include <service-cellular/CellularMessage.hpp>
#include "ticks.hpp"
#include <utility>
#include <vector>

using namespace std::chrono_literals;

ATParser::ATParser(bsp::Cellular *cellular) : Channel{new uint8_t[at::defaultReceiveBufferSize]}, cellular(cellular)
{
    assert(cellular != nullptr);

    responseBuffer = xMessageBufferCreate(at::defaultMessageBufferSize);
}

/// plz see 12.7 summary of urc in documentation
std::vector<ATParser::Urc> ATParser::parseUrc()
{
    size_t maxPos = 0, pos = 0;

    std::vector<std::pair<std::string, ATParser::Urc>> vals = {
        {"RDY", ATParser::Urc::MeInitializationSuccessful},
        {"+CFUN: 1", ATParser::Urc::FullFuncionalityAvailable},
    };

    std::vector<ATParser::Urc> resp;
    resp.reserve(vals.size() + 1);
    cpp_freertos::LockGuard lock(mutex);

    for (const auto &el : vals) {
        pos = urcBuffer.find(el.first);
        if (pos != std::string::npos) {
            resp.push_back(el.second);
            maxPos = std::max(pos + el.first.length(), maxPos);
            LOG_SENSITIVE(LOGDEBUG, "%s", ("[URC]: " + el.first).c_str());
        }
    }

    if (urcBuffer.find("+QIND: \"FOTA\"") != std::string::npos) {
        resp.push_back(ATParser::Urc::Fota);
        return resp;
    }

    // manage string buffer
    if (maxPos == 0) {}
    else if (urcBuffer.size() >= maxPos) {
        urcBuffer.erase();
    }
    else {
        urcBuffer = urcBuffer.substr(maxPos);
    }

    return resp;
}

at::Result ATParser::processNewData(sys::Service *service, const bsp::cellular::CellularResult &cellularResult)
{
    at::Result result;

    {
        cpp_freertos::LockGuard lock(mutex);
        urcBuffer.append(cellularResult.getDataAsString());
    }

    auto ret = parseUrc();

    if (awaitingResponseFlag.state()) {
        if (!xMessageBufferSend(responseBuffer,
                                cellularResult.getSerialized().get(),
                                cellularResult.getSerializedSize(),
                                pdMS_TO_TICKS(at::defaultBufferTimeoutMs.count()))) {
            LOG_DEBUG("[AT] Message buffer full!");
            result.code = at::Result::Code::FULL_MSG_BUFFER;
        }
    }
    else if (!ret.empty()) {
        if (ret.size() == 1 && ret[0] == ATParser::Urc::Fota) {
            std::string fotaData;
            {
                cpp_freertos::LockGuard lock(mutex);
                fotaData = std::string(urcBuffer);
                urcBuffer.erase();
            }
            LOG_DEBUG("parsing FOTA");
            FotaService::API::sendRawProgress(service, fotaData);
        }
        else {
            urcs.insert(std::end(urcs), std::begin(ret), std::end(ret));
        }
        // GSM modem is considered as operational when it outputs URCs specified below:
        // 1) RDY
        // 2) +CFUN: 1
        if (urcs.size() == 2) {
            auto msg = std::make_shared<CellularPowerUpProcedureCompleteNotification>();
            service->bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceCellularNotifications);
            urcs.clear();

            cpp_freertos::LockGuard lock(mutex);
            urcBuffer.erase();
        }
    }
    else {
        result.code = at::Result::Code::DATA_NOT_USED;
    }

    return result;
}

void ATParser::cmdInit()
{
    cpp_freertos::LockGuard lock(mutex);
    urcBuffer.erase();
}

void ATParser::cmdSend(std::string cmd)
{
    cellular->write(const_cast<char *>(cmd.c_str()), cmd.size());
}

size_t ATParser::cmdReceive(std::uint8_t *buffer, std::chrono::milliseconds timeout = 0ms)
{
    return xMessageBufferReceive(responseBuffer, buffer, 256, pdMS_TO_TICKS(timeout.count()));
}

void ATParser::cmdPost()
{
    cpp_freertos::LockGuard lock(mutex);
    urcBuffer.erase();
    xMessageBufferReset(responseBuffer);
}
