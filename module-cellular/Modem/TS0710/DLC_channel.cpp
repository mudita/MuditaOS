// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DLC_channel.h"

#include "TS0710_DATA.h"
#include "TS0710_DLC_RELEASE.h"
#include "TS0710_Frame.h"

#include <module-utils/log/log.hpp>
#include <ticks.hpp>
#include <Utils.hpp>

DLC_channel::DLC_channel(DLCI_t DLCI, const std::string &name, bsp::Cellular *cellular, const Callback_t &callback)
    : Channel{new uint8_t[at::defaultReceiveBufferSize]}, pv_name{name}, pv_DLCI{DLCI}, pv_cellular{cellular}
{
    LOG_DEBUG("Creating DLCI %i channel \"%s\"", DLCI, name.c_str());

    if (callback != nullptr) {
        pv_callback = callback;
    }

    chanParams.TypeOfFrame             = TypeOfFrame_e::SABM;
    chanParams.ConvergenceLayer        = 1;
    chanParams.Priority                = 1;
    chanParams.AckTime                 = 100; // 100ms default
    chanParams.MaxFrameSize            = 128;
    chanParams.MaxNumOfRetransmissions = 3; // default 3
    chanParams.ErrRecovWindowSize      = 2; // default 2

    responseBuffer = xMessageBufferCreate(at::defaultMessageBufferSize);
}

bool DLC_channel::init()
{
    active = establish();
    LOG_INFO("create channel %s: %s", pv_name.c_str(), active ? "TRUE" : "FALSE");

    return active;
}

DLC_channel::~DLC_channel()
{
    TS0710_DLC_RELEASE release = TS0710_DLC_RELEASE(pv_DLCI);
}

void DLC_channel::SendData(std::vector<uint8_t> &data)
{
    TS0710_DATA _data = TS0710_DATA(pv_DLCI, chanParams, data, pv_cellular);
}

bool DLC_channel::establish()
{
    LOG_DEBUG("Sending %s frame to DLCI %i", TypeOfFrame_text[chanParams.TypeOfFrame].c_str(), pv_DLCI);

    TS0710_Frame frame_c(TS0710_Frame::frame_t(static_cast<uint8_t>(pv_DLCI << 2) | (1 << 1),
                                               static_cast<uint8_t>(chanParams.TypeOfFrame)));

    awaitingResponseFlag.set();

    bool result = false;

    for (int retries = 0; retries < chanParams.MaxNumOfRetransmissions; ++retries) {
        pv_cellular->write(static_cast<void *>(frame_c.getSerData().data()), frame_c.getSerData().size());

        auto startTime = std::chrono::steady_clock::now();
        auto endTime   = startTime + std::chrono::milliseconds{300};

        // Wait for response:
        while (true) {
            if (std::chrono::steady_clock::now() > endTime) {
                break;
            }

            if (size_t bytesRead = cmd_receive(receiveBuffer.get(), std::chrono::milliseconds{0}); bytesRead > 0) {
                auto cellularResult = bsp::cellular::CellularResult{receiveBuffer.get(), bytesRead};
                if (evaluateEstablishResponse(cellularResult)) {
                    result = true;
                    break;
                }
            }
        }
    }

    awaitingResponseFlag.clear();

    return result;
}

void DLC_channel::cmd_init()
{}

void DLC_channel::cmd_send(std::string cmd)
{
    std::vector<uint8_t> data(cmd.begin(), cmd.end());
    SendData(data);
}

size_t DLC_channel::cmd_receive(uint8_t *result, std::chrono::milliseconds timeout)
{
    return xMessageBufferReceive(responseBuffer, result, 2 * chanParams.MaxFrameSize, pdMS_TO_TICKS(timeout.count()));
}

void DLC_channel::cmd_post()
{}

std::vector<std::string> DLC_channel::SendCommandPrompt(const char *cmd,
                                                        size_t rxCount,
                                                        std::chrono::milliseconds timeout)
{
    std::vector<std::string> tokens;

    awaitingResponseFlag.set();

    at::Result result;

    cmd_init();
    std::string cmdFixed = formatCommand(cmd);
    cmd_send(cmdFixed);

    auto startTime = std::chrono::steady_clock::now();
    auto endTime   = startTime + timeout;

    // Wait for response:
    while (true) {
        if (std::chrono::steady_clock::now() > endTime) {
            result.code = at::Result::Code::TIMEOUT;
            break;
        }

        if (size_t bytesRead = cmd_receive(receiveBuffer.get(), std::chrono::milliseconds{0}); bytesRead > 0) {
            auto cellularResult = bsp::cellular::CellularResult{receiveBuffer.get(), bytesRead};
            auto str            = cellularResult.getDataAsString();

            if (result = checkResult(cellularResult.getResultCode()); result.code != at::Result::Code::OK) {
                break;
            }

            // tokenize responseBuffer
            auto pos = str.find('>');
            if (pos != std::string::npos) {
                tokens.push_back(str.substr(pos, strlen(">")));
                break;
            }
            if (tokens.size() >= rxCount) {
                break;
            }
        }
    }

    cmdLog(cmdFixed, result, timeout);
    cmd_post();

    awaitingResponseFlag.clear();

    return tokens;
}

at::Result DLC_channel::ParseInputData(bsp::cellular::CellularResult *cellularResult)
{
    at::Result result;

    if (awaitingResponseFlag.state()) {
        if (!xMessageBufferSend(responseBuffer,
                                (void *)cellularResult->getSerialized().get(),
                                cellularResult->getSerializedSize(),
                                pdMS_TO_TICKS(at::defaultBufferTimeoutMs.count()))) {
            LOG_DEBUG("[DLC] Message buffer full!");
            result.code = at::Result::Code::FULL_MSG_BUFFER;
        }
    }
    else if (pv_callback != nullptr) {
        std::string receivedData = cellularResult->getDataAsString();
        pv_callback(receivedData);
    }
    else {
        result.code = at::Result::Code::DATA_NOT_USED;
    }

    return result;
}

bool DLC_channel::evaluateEstablishResponse(bsp::cellular::CellularResult &response) const
{
    auto frame = TS0710_Frame{response.getData()};
    return (frame.getFrameDLCI() == pv_DLCI &&
            (frame.getFrame().Control == (static_cast<uint8_t>(TypeOfFrame_e::UA) & ~(1 << 4))));
}
