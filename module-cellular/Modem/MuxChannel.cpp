
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

}


MuxChannel::MuxChannel(MuxDaemon *mux, uint32_t logicalNumber, MuxChannelType type, const char *name,
                       uint32_t stackSize, uint32_t queueSize) :
        v24signals(
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_DV ) | static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTR) |
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTC) | static_cast<int >(MuxDefines::GSM0710_EA)),
        frameAllowed(1),
        disc_ua_pending(0),
        workerStackSize(stackSize),
        state(State::Init),
        type(type),
        name(name),
        mux(mux){


}

MuxChannel::~MuxChannel() {

    vTaskDelete(workerHandle);
    vStreamBufferDelete(inputBuffer);
}

int MuxChannel::Open() {

    enableWorkerLoop = true;
    inputBuffer = xStreamBufferCreate(inputBufferSize,1);
    xTaskCreate(MuxChannelWorker, (name + "_Worker").c_str(), workerStackSize / 4, this, 0, &workerHandle);


    state = State ::Opening;

    // Send virtual channel request frame to GSM modem
    mux->WriteMuxFrame(GetChannelNumber(), NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_SABM) |
                                               static_cast<unsigned char>(MuxDefines::GSM0710_PF));

    return 0;
}

int MuxChannel::Close() {

    state = State ::Closing;

    mux->WriteMuxFrame(GetChannelNumber(), NULL, 0,
                       static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_DISC)|
                       static_cast<unsigned char>(MuxDefines::GSM0710_PF));

    return 0;
}


ssize_t MuxChannel::SendData(uint8_t *data, size_t size) {
    return xStreamBufferSend(inputBuffer,data,size,100);
}

std::vector<std::string> MuxChannel::Tokenizer(std::string &input, uint32_t maxTokenCount,
                                               const std::string &delimiter) {
    std::vector<std::string> strings;
    uint32_t tokenCount = 0;


    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while (((pos = input.find(delimiter, prev)) != std::string::npos) && (tokenCount<maxTokenCount))
    {
        if(pos == prev){
            prev = pos + delimiter.size();
            continue;
        }

        strings.push_back(input.substr(prev, pos - prev));
        prev = pos + delimiter.size();
        tokenCount++;
    }

    // To get the last substring (or only, if delimiter is not found)
    input = input.substr(prev);
    return strings;
}
