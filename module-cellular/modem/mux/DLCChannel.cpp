// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DLCChannel.h"

#include "CellularMuxData.h"
#include "CellularMuxFrame.h"

#include <log/log.hpp>
#include <ticks.hpp>
#include <Utils.hpp>
#include <magic_enum.hpp>

DLCChannel::DLCChannel(DLCI_t DLCI, const std::string &name, bsp::Cellular *cellular, const Callback_t &callback)
    : Channel{new uint8_t[at::defaultReceiveBufferSize]}, name{name}, DLCI{DLCI}, pvCellular{cellular}
{
    LOG_DEBUG("Creating DLCI %i channel \"%s\"", DLCI, name.c_str());

    if (callback != nullptr) {
        pvCallback = callback;
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

bool DLCChannel::init()
{
    active = establish();
    LOG_INFO("create channel %s: %s", name.c_str(), active ? "TRUE" : "FALSE");

    return active;
}

void DLCChannel::sendData(std::vector<uint8_t> &data)
{
    CellularMuxData(DLCI, chanParams, data, pvCellular);
}

bool DLCChannel::establish()
{
    LOG_SENSITIVE(LOGDEBUG, "Sending %s frame to DLCI %i", TypeOfFrame_text[chanParams.TypeOfFrame].c_str(), DLCI);

    CellularMuxFrame frame_c(CellularMuxFrame::frame_t(static_cast<uint8_t>(DLCI << 2) | (1 << 1),
                                                       static_cast<uint8_t>(chanParams.TypeOfFrame)));

    awaitingResponseFlag.set();

    bool result = false;

    for (int retries = 0; retries < chanParams.MaxNumOfRetransmissions; ++retries) {
        pvCellular->write(static_cast<void *>(frame_c.getSerData().data()), frame_c.getSerData().size());

        auto startTime = std::chrono::steady_clock::now();
        auto endTime   = startTime + std::chrono::milliseconds{300};

        // Wait for response:
        while (true) {
            if (std::chrono::steady_clock::now() > endTime) {
                break;
            }

            if (size_t bytesRead = cmdReceive(receiveBuffer.get(), std::chrono::milliseconds{0}); bytesRead > 0) {
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

void DLCChannel::cmdInit()
{}

int DLCChannel::cmdSend(std::string cmd)
{
    std::vector<uint8_t> data(cmd.begin(), cmd.end());
    sendData(data);
    return 0;
}

size_t DLCChannel::cmdReceive(uint8_t *result, std::chrono::milliseconds timeout)
{
    return xMessageBufferReceive(responseBuffer, result, 2 * chanParams.MaxFrameSize, pdMS_TO_TICKS(timeout.count()));
}

void DLCChannel::cmdPost()
{}

std::vector<std::string> DLCChannel::sendCommandPrompt(const char *cmd,
                                                       size_t rxCount,
                                                       std::chrono::milliseconds timeout)
{
    std::vector<std::string> tokens;

    awaitingResponseFlag.set();

    at::Result result;

    cmdInit();
    std::string cmdFixed = formatCommand(cmd);
    cmdSend(cmdFixed);

    auto startTime = std::chrono::steady_clock::now();
    auto endTime   = startTime + timeout;

    // Wait for response:
    while (true) {
        if (std::chrono::steady_clock::now() > endTime) {
            result.code = at::Result::Code::TIMEOUT;
            break;
        }

        if (size_t bytesRead = cmdReceive(receiveBuffer.get(), std::chrono::milliseconds{0}); bytesRead > 0) {
            auto cellularResult = bsp::cellular::CellularResult{receiveBuffer.get(), bytesRead};
            auto str            = cellularResult.getDataAsString();

            auto cellResult = checkResult(cellularResult.getResultCode());
            if (cellResult.code != at::Result::Code::OK) {
                LOG_DEBUG("SendCommandPrompt cmd response code: %s",
                          std::string(magic_enum::enum_name(cellResult.code)).c_str());
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
    cmdPost();

    awaitingResponseFlag.clear();

    return tokens;
}

at::Result DLCChannel::parseInputData(bsp::cellular::CellularResult *cellularResult) const
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
    else if (pvCallback != nullptr) {
        std::string receivedData = cellularResult->getDataAsString();
        pvCallback(receivedData);
    }
    else {
        result.code = at::Result::Code::DATA_NOT_USED;
    }

    return result;
}

bool DLCChannel::evaluateEstablishResponse(bsp::cellular::CellularResult &response) const
{
    auto frame = CellularMuxFrame{response.getData()};
    return (frame.getFrameDLCI() == DLCI &&
            (frame.getFrame().Control == (static_cast<uint8_t>(TypeOfFrame_e::UA) & ~(1 << 4))));
}
