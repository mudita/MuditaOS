// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * Project Untitled
 */

#ifndef _DLC_CHANNEL_H
#define _DLC_CHANNEL_H

#include <string>
#include <vector>
#include <functional>

#include "Modem/ATCommon.hpp"
#include "TS0710_types.h"
#include <bsp/cellular/CellularResult.hpp>
#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>

class DLC_channel : public at::Channel
{
  public:
    using Callback_t = std::function<void(std::string &data)>;

  private:
    std::string pv_name;
    DLCI_t pv_DLCI;
    bool active = false;
    DLC_ESTABL_SystemParameters_t chanParams{};
    Callback_t pv_callback;
    bsp::Cellular *pv_cellular{};

  public:
    DLC_channel(DLCI_t DLCI, const std::string &name, bsp::Cellular *cellular, const Callback_t &callback = nullptr);
    DLC_channel() : Channel{nullptr}, pv_name{"none"}, pv_DLCI{-1}
    {}

    virtual ~DLC_channel();

    bool init();
    bool establish();

    void SendData(std::vector<uint8_t> &data);

    DLCI_t getDLCI()
    {
        return pv_DLCI;
    }
    std::string getName()
    {
        return pv_name;
    }
    bool getActive()
    {
        return active;
    }

    void setCallback(Callback_t callback)
    {
        LOG_DEBUG("[%s] Setting up callback for channel", pv_name.c_str());
        pv_callback = callback;
    }

    virtual void cmd_init() override final;
    virtual void cmd_send(std::string cmd) override final;
    virtual size_t cmd_receive(std::uint8_t *result,
                               std::chrono::milliseconds timeout = std::chrono::milliseconds{300}) override final;
    virtual void cmd_post() override final;

    std::vector<std::string> SendCommandPrompt(const char *cmd,
                                               size_t rxCount,
                                               std::chrono::milliseconds timeout = std::chrono::milliseconds{300});

    at::Result ParseInputData(bsp::cellular::CellularResult *cellularResult);

    bool evaluateEstablishResponse(bsp::cellular::CellularResult &response) const;

    void callback(std::string &data)
    {
        pv_callback(data);
    }
};

#endif //_DLC_CHANNEL_H
