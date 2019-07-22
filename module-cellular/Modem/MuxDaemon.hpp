
/*
 * @file MuxDaemon.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 19.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_MUXDAEMON_HPP
#define PUREPHONE_MUXDAEMON_HPP

#include <vector>
#include <optional>
#include <stdint.h>
#include "Service/Worker.hpp"
#include "NotificationMuxChannel.hpp"
#include "MuxChannel.hpp"
#include "GSM0710.hpp"

class InOutSerialWorker;

namespace bsp{
    class Cellular;
}

class MuxDaemon {

public:

    /**
     * Multiplexer configuration
     */

    const static uint32_t baudRate = 115200;
    const static uint32_t virtualPortsCount = 3;
    const static bool hardwareControlFlowEnable = false;

    /**
     * Multiplexer states
     */
    enum class States {
        MUX_STATE_POWERUP,
        MUX_STATE_OPENING,
        MUX_STATE_INITILIZING,
        MUX_STATE_MUXING,
        MUX_STATE_CLOSING,
        MUX_STATE_OFF,
        MUX_STATES_COUNT // keep this the last
    };

    static std::unique_ptr<MuxDaemon> Create(NotificationMuxChannel::NotificationCallback_t callback);

    MuxDaemon(NotificationMuxChannel::NotificationCallback_t callback);

    ~MuxDaemon();

    std::vector<std::string> SendCommandResponse(MuxChannel::MuxChannelType type, const char *cmd, size_t rxCount,
                                                 uint32_t timeout = 1000);

    inline MuxDaemon::States GetState(){return state;}
    inline void SetState(MuxDaemon::States st){state = st;}

    std::optional<MuxChannel*> GetMuxChannel(MuxChannel::MuxChannelType chan);
    void RemoveMuxChannel(MuxChannel::MuxChannelType chan);

    bool PowerUpProcedure();
    bool StartMultiplexer();

    int uihPfBitReceived = 0;

    NotificationMuxChannel::NotificationCallback_t callback = nullptr;

private:

    friend void workerTaskFunction(void *ptr);

    bool Start();

    int CloseMultiplexer();

    bool CheckATCommandResponse(const std::vector<std::string>& response);


    constexpr static unsigned char closeChannelCmd[] = {
            static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD ) |
            static_cast<unsigned char>(MuxDefines::GSM0710_CR),
            static_cast<unsigned char>(MuxDefines::GSM0710_EA ) | (0 << 1)};


    std::unique_ptr<InOutSerialWorker> inOutSerialDataWorker;


    States state = States::MUX_STATE_OPENING;

    std::vector<std::unique_ptr<MuxChannel>> channels;

    std::unique_ptr<bsp::Cellular> cellular;


};


#endif //PUREPHONE_MUXDAEMON_HPP
