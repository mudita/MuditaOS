/**
 * Project Untitled
 */

#ifndef _DLC_CHANNEL_H
#define _DLC_CHANNEL_H

#include <string>
#include <vector>
#include <functional>

#include "../ATCommon.hpp"
#include "TS0710_types.h"
#include <FreeRTOS.h>
#include <task.h>

class DLC_channel : public at::Chanel
{
  public:
    using Callback_t = std::function<void(std::vector<uint8_t> &data)>;

  private:
    std::string pv_name;
    DLCI_t pv_DLCI;
    bool active = false;
    DLC_ESTABL_SystemParameters_t pv_chanParams;
    Callback_t pv_callback;

    std::string responseBuffer;
    bsp::Cellular *pv_cellular;

  public:
    // TS0710_DLC_ESTABL ctrlChanEstabl = TS0710_DLC_ESTABL(0);  //use default values to create control channel DLCI0

    DLC_channel(DLCI_t DLCI, std::string name, bsp::Cellular *cellular, Callback_t callback = nullptr);
    DLC_channel()
    {
        pv_DLCI = -1;
        pv_name = "none";
    } // default constructor creates empty channel
    virtual ~DLC_channel();

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

    // ssize_t ReceiveData(std::vector<uint8_t> &data, uint32_t timeout);
    void setCallback(Callback_t callback)
    {
        LOG_DEBUG("[%s] Setting up callback for channel", pv_name.c_str());
        pv_callback = callback;
    }

    virtual void cmd_init() override final;
    virtual void cmd_send(std::string cmd) override final;
    virtual std::vector<std::string> cmd_receive() override final;
    virtual void cmd_post() override final;

    std::vector<std::string> SendCommandPrompt(const char *cmd, size_t rxCount, uint32_t timeout = 300);

    int ParseInputData(std::vector<uint8_t> &data);

    void callback(std::vector<uint8_t> &data)
    {
        pv_callback(data);
    }
};

#endif //_DLC_CHANNEL_H
