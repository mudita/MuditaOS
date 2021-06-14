// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>

#include <bsp/cellular/CellularResult.hpp>

#include "modem/ATCommon.hpp"
#include "CellularMuxTypes.h"

class DLCChannel : public at::Channel
{
  public:
    using Callback_t = std::function<void(std::string &data)>;

  private:
    std::string name;
    DLCI_t DLCI = -1;
    bool active = false;
    DLC_ESTABL_SystemParameters_t chanParams{};
    Callback_t pvCallback;
    bsp::Cellular *pvCellular = nullptr;

  public:
    DLCChannel(DLCI_t DLCI, const std::string &name, bsp::Cellular *cellular, const Callback_t &callback = nullptr);
    DLCChannel() : Channel{nullptr}, name{"none"}, DLCI{-1} {};

    bool init();
    bool establish();
    void sendData(std::vector<uint8_t> &data);

    virtual void cmdInit() override final;
    virtual void cmdSend(std::string cmd) override final;
    virtual size_t cmdReceive(std::uint8_t *result, std::chrono::milliseconds timeout) override final;
    virtual void cmdPost() override final;

    std::vector<std::string> sendCommandPrompt(const char *cmd,
                                               size_t rxCount,
                                               std::chrono::milliseconds timeout = std::chrono::milliseconds{300});

    at::Result parseInputData(bsp::cellular::CellularResult *cellularResult) const;

    bool evaluateEstablishResponse(bsp::cellular::CellularResult &response) const;

    void callback(std::string &data)
    {
        pvCallback(data);
    }

    DLCI_t getDLCI() const noexcept
    {
        return DLCI;
    }
    std::string getName() const
    {
        return name;
    }
    bool getActive() const noexcept
    {
        return active;
    }

    void setCallback(Callback_t callback)
    {
        LOG_DEBUG("[%s] Setting up callback for channel", name.c_str());
        pvCallback = std::move(callback);
    }
};
