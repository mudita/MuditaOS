
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

#include "ticks.hpp"


std::optional<std::unique_ptr<InOutSerialWorker>> InOutSerialWorker::Create(MuxDaemon *mux) {
    auto inst = std::make_unique<InOutSerialWorker>(mux);

    if (inst->isInitialized) {
        return inst;
    } else {
        return {};
    }
}

InOutSerialWorker::InOutSerialWorker(MuxDaemon *mux) : muxDaemon(mux), atParser(nullptr), muxParser(nullptr) {

    // Create and initialize bsp::Cellular module
    if ((cellular = bsp::Cellular::Create(SERIAL_PORT).value_or(nullptr)) == nullptr) {
        return;
    }

    if((muxParser = MuxParser::Create(muxDaemon,this,cellular.get()).value_or(nullptr)) == nullptr){
        return;
    }

    if((atParser = ATParser::Create(muxDaemon,this,cellular.get()).value_or(nullptr)) == nullptr){
        return;
    }

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
            inst->atParser->ProcessNewData();
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

std::vector<std::string> InOutSerialWorker::SendATCommand(const char* cmd,size_t rxCount,uint32_t timeout) {

    return atParser->SendCommand(cmd,rxCount,timeout);

/*    char buff[256] = {0};

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
    }*/
}

ssize_t InOutSerialWorker::SendFrame(int channel, const unsigned char *input, int length, unsigned char type) {
    return muxParser->SendMuxFrame(channel,input,length,type);
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

