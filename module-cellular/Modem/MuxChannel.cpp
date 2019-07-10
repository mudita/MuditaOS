
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

    while (inst->enableWorkerLoop) {

        uint8_t buffer[MuxChannel::inputBufferSize] = {0};
        auto bytesReceived = xStreamBufferReceive(inst->inputBuffer, buffer,MuxChannel::inputBufferSize,pdMS_TO_TICKS(portMAX_DELAY));

        // Check if incoming data is actually signal to close
        if((inst->enableWorkerLoop == false) && (buffer[0] == 0) && (bytesReceived == 1)){
            goto exit;
        }

        switch (inst->type) {

            case MuxChannel::MuxChannelType::Notification: {
                NotificationMuxChannel *muxChan = reinterpret_cast<NotificationMuxChannel *>(pvp);
                muxChan->ParseInputData(buffer,bytesReceived);
            }
                break;

            case MuxChannel::MuxChannelType ::Communication:{
                CommunicationMuxChannel *muxChan = reinterpret_cast<CommunicationMuxChannel *>(pvp);
                muxChan->ParseInputData(buffer,bytesReceived);
                break;
            }
            default:
                // The rest of mux channels aren't currently supported
                inst->ParseInputData(buffer,bytesReceived);;
                break;
        }
    }

    exit:
    LOG_DEBUG("Closing worker thread.");
    vStreamBufferDelete(inst->inputBuffer);

    vTaskDelete(NULL);
}


MuxChannel::MuxChannel(MuxDaemon *mux, uint32_t logicalNumber, MuxChannelType type, const char *name,
                       uint32_t stackSize, uint32_t queueSize) :
        v24signals(
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_DV ) | static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTR) |
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTC) | static_cast<int >(MuxDefines::GSM0710_EA)),
        frameAllowed(1),
        disc_ua_pending(0),
        workerStackSize(stackSize),
        state(State::Closed),
        type(type),
        name(name),
        mux(mux){


}

MuxChannel::~MuxChannel() {

}

int MuxChannel::Open() {

    enableWorkerLoop = true;
    inputBuffer = xStreamBufferCreate(inputBufferSize,1);
    xTaskCreate(MuxChannelWorker, (name + "_Worker").c_str(), workerStackSize / 4, this, 0, &workerHandle);


    // Send virtual channel request frame to GSM modem
    mux->WriteMuxFrame(GetChannelNumber(), NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_SABM) |
                                               static_cast<unsigned char>(MuxDefines::GSM0710_PF));

    return 0;
}

int MuxChannel::Close() {

    enableWorkerLoop = false;

    uint8_t dummy = 0;
    // Send dummy data to kick worker thread out of suspend state
    SendData(&dummy,sizeof dummy);

    mux->WriteMuxFrame(GetChannelNumber(), NULL, 0,
                       static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_DISC));

    return 0;
}


ssize_t MuxChannel::SendData(uint8_t *data, size_t size) {
    return xStreamBufferSend(inputBuffer,data,size,100);
}

int MuxChannel::ParseInputData(uint8_t *data, size_t size){
    //LOG_FATAL((name + " received message: " + msg->m_data).c_str());
    return 0;
}