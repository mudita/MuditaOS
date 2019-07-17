
/*
 * @file InOutSerialWorker.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "InOutSerialWorker.hpp"
#include "MuxDaemon.hpp"
#include "log/log.hpp"
#include "cellular/bsp_cellular.hpp"
#include "MuxParser.hpp"
#include "ATParser.hpp"


std::optional<std::unique_ptr<InOutSerialWorker>> InOutSerialWorker::Create(MuxDaemon *mux) {
    auto inst = std::make_unique<InOutSerialWorker>(mux);

    if (inst->isInitialized) {
        return inst;
    } else {
        return {};
    }
}

InOutSerialWorker::InOutSerialWorker(MuxDaemon *mux) : muxDaemon(mux), atParser(nullptr), muxParser(nullptr) {

    BaseType_t task_error = xTaskCreate(
            workerTaskFunction,
            "cellInSerWorker",
            512, // in words
            this,
            taskPriority,
            &taskHandle);
    if (task_error != pdPASS) {
        LOG_ERROR("Failed to start inputSerialWorker task");
        return;
    }

    // Create and initialize bsp::Cellular module
    if (cellular = bsp::Cellular::Create(SERIAL_PORT).value_or(nullptr)) {
        return;
    }

    if(muxParser = MuxParser::Create(muxDaemon,this,cellular.get()).value_or(nullptr)){
        return;
    }

    //TODO:M.P create ATParser

    isInitialized = true;
}

InOutSerialWorker::~InOutSerialWorker() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
    }
}


void workerTaskFunction(void *ptr) {
    InOutSerialWorker *inst = reinterpret_cast<InOutSerialWorker *>(ptr);

    while (1) {
        inst->cellular->Wait(UINT32_MAX);

        // AT mode is used only during initialization phase
        if (inst->mode == InOutSerialWorker::Mode::AT) {

        }
        // CMUX mode is default operation mode
        else {
            inst->muxParser->ProcessNewData();
        }
    }
}


void InOutSerialWorker::SwitchMode(const InOutSerialWorker::Mode newMode) {
    mode = newMode;
}

ssize_t InOutSerialWorker::WriteData(unsigned char *input, size_t length) {
    //TODO: M.P implement actual caching
    cpp_freertos::LockGuard lock(serOutMutex);
    return cellular->Write(input, length);
}

int InOutSerialWorker::SendATCommand(const char *cmd, uint32_t timeout) {
    char buff[256] = {0};

    auto bytesWritten = cellular->Write(const_cast<char *>(cmd), strlen(cmd));
    LOG_DEBUG("BytesWritten: %d", bytesWritten);

    if (cellular->Wait(500)) {

        vTaskDelay(100);
        auto bytesRead = cellular->Read(buff, sizeof buff);

        if (memstr((char *) buff, bytesRead, "OK")) {
            LOG_DEBUG("Received OK");
            return 0;
        } else if (memstr((char *) buff, bytesRead, "ERROR")) {
            LOG_DEBUG("Received ERROR");
            return -1;
        } else {
            LOG_DEBUG("Received unknown response");
            return -1;
        }
    } else {
        return -1;
    }
}

ssize_t InOutSerialWorker::SendMuxFrame(int channel, const unsigned char *input, int length, unsigned char type) {
    /* flag, GSM0710_EA=1 C channel, frame type, length 1-2 */
    unsigned char prefix[5] = {static_cast<unsigned char>(MuxDefines::GSM0710_FRAME_FLAG),
                               static_cast<unsigned char>(MuxDefines::GSM0710_EA) |
                               static_cast<unsigned char>(MuxDefines::GSM0710_CR), 0, 0, 0};
    unsigned char postfix[2] = {0xFF, static_cast<unsigned char>(MuxDefines::GSM0710_FRAME_FLAG )};
    ssize_t prefix_length = 4;
    int c;
    unsigned char tmp[GSM0710Buffer::cmux_FRAME];

    LOG_DEBUG("Sending frame to channel %d", channel);

    /* GSM0710_EA=1, Command, let's add address */
    prefix[1] = prefix[1] | ((63 & (unsigned char) channel) << 2);

/* let's set control field */
    prefix[2] = type;
    if ((type == static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH) ||
         type == static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UI)) &&
            muxDaemon->uihPfBitReceived == 1 &&
        GSM0710Buffer::GSM0710_COMMAND_IS(MuxDefines::GSM0710_CONTROL_MSC, input[0])) {
        prefix[2] = prefix[2] |
                    static_cast<unsigned char>(MuxDefines::GSM0710_PF); //Set the P/F bit in Response if Command from modem had it set
        muxDaemon->uihPfBitReceived = 0; //Reset the variable, so it is ready for next command
    }
/* let's not use too big frames */
    length = std::min(GSM0710Buffer::cmux_N1, static_cast<uint32_t >(length));
    // Only basic mode is supported
    {
/* Modified acording PATCH CRC checksum */
/* postfix[0] = frame_calc_crc (prefix + 1, prefix_length - 1); */
/* length */
        if (length > 127) {
            prefix_length = 5;
            prefix[3] = (0x007F & length) << 1;
            prefix[4] = (0x7F80 & length) >> 7;
        } else {
            prefix[3] = 1 | (length << 1);
        }
        postfix[0] = GSM0710Buffer::frameCalcCRC(prefix + 1, prefix_length - 1);

        memcpy(tmp, prefix, prefix_length);

        if (length > 0) {
            memcpy(tmp + prefix_length, input, length);
        }

        memcpy(tmp + prefix_length + length, postfix, 2);
        c = prefix_length + length + 2;

        //Write newly created frame into serial output buffer
        WriteData(tmp, c);
    }

    return length;
}

int InOutSerialWorker::memstr(const char *haystack, int length, const char *needle) {
    int i;
    int j = 0;
    if (needle[0] == '\0')
        return 1;
    for (i = 0; i < length; i++)
        if (needle[j] == haystack[i]) {
            j++;
            if (needle[j] == '\0') // Entire needle was found
                return 1;
        } else
            j = 0;
    return 0;
}