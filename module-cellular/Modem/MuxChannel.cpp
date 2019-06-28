
/*
 * @file MuxChannel.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "MuxChannel.hpp"
#include "GSM0710.hpp"
#include "MuxDaemon.hpp"
#include "NotificationMuxChannel.hpp"
#include "CommunicationMuxChannel.hpp"

void MuxChannelWorker(void *pvp) {
    MuxChannel *inst = reinterpret_cast<MuxChannel *>(pvp);

    while (1) {

        MuxChannel::MuxChannelMsg *msgReceived = nullptr;
        xQueueReceive(inst->workerQueueHandle, &msgReceived, portMAX_DELAY);

        switch (inst->type) {

            case MuxChannel::MuxChannelType::Notification: {
                NotificationMuxChannel *muxChan = reinterpret_cast<NotificationMuxChannel *>(pvp);
                muxChan->ParseInMessage(msgReceived);
            }
                break;

            case MuxChannel::MuxChannelType ::Communication:{
                CommunicationMuxChannel *muxChan = reinterpret_cast<CommunicationMuxChannel *>(pvp);
                muxChan->ParseInMessage(msgReceived);
                break;
            }
            default:
                // The rest of mux channels aren't currently supported
                inst->ParseInMessage(msgReceived);
                break;
        }

        delete (msgReceived);

    }
}


MuxChannel::MuxChannel(MuxDaemon *mux, uint32_t logicalNumber, MuxChannelType type, const char *name,
                       uint32_t stackSize, uint32_t queueSize) :
        v24signals(
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_DV ) | static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTR) |
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTC) | static_cast<int >(MuxDefines::GSM0710_EA)),
        frameAllowed(1),
        disc_ua_pending(0),
        workerQueueSize(queueSize),
        workerStackSize(stackSize),
        state(State::Closed),
        type(type),
        name(name),
        mux(mux){


}

MuxChannel::~MuxChannel() {

}

int MuxChannel::Open() {

    workerQueueHandle = xQueueCreate(workerQueueSize, sizeof(MuxChannelMsg *));
    xTaskCreate(MuxChannelWorker, (name + "_Worker").c_str(), workerStackSize / 4, this, 0, &workerHandle);

    // Send virtual channel request frame to GSM modem
    mux->WriteMuxFrame(GetChannelNumber(), NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_SABM) |
                                               static_cast<unsigned char>(MuxDefines::GSM0710_PF));

    return 0;
}

int MuxChannel::Close() {

    vQueueDelete(workerQueueHandle);
    vTaskDelete(workerHandle);

    if (mux->inputBuffer->cmux_mode)
        mux->WriteMuxFrame(GetChannelNumber(), NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD) |
                                                   static_cast<unsigned char>(MuxDefines::GSM0710_CR));
    else
        mux->WriteMuxFrame(GetChannelNumber(), mux->closeChannelCmd, sizeof(mux->closeChannelCmd),
                           static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));

    return 0;
}


int MuxChannel::Send(uint8_t *data, size_t size) {

    MuxChannelMsg *msg = new MuxChannelMsg(data, size);
    xQueueSend(workerQueueHandle, &msg, 500);
    return size;
}

int MuxChannel::ParseInMessage(MuxChannel::MuxChannelMsg *msg) {
    LOG_FATAL((name + " received message: " + msg->m_data).c_str());
    return 0;
}