
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
#include "bsp/cellular/bsp_cellular.hpp"
#include "MuxParser.hpp"
#include "ATParser.hpp"

#include "ticks.hpp"


std::optional<std::unique_ptr<InOutSerialWorker>> InOutSerialWorker::Create(MuxDaemon *mux, bsp::Cellular *cellular) {
    auto inst = std::make_unique<InOutSerialWorker>(mux,cellular);

    if (inst->isInitialized) {
        return inst;
    } else {
        return {};
    }
}

InOutSerialWorker::InOutSerialWorker(MuxDaemon *mux, bsp::Cellular *cellular) : muxDaemon(mux), atParser(nullptr),
                                                                                muxParser(nullptr), cellular(cellular) {

    if ((muxParser = MuxParser::Create(muxDaemon, this, cellular).value_or(nullptr)) == nullptr) {
        return;
    }

    if ((atParser = ATParser::Create(muxDaemon, this, cellular).value_or(nullptr)) == nullptr) {
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
        auto ret = inst->cellular->Wait(UINT32_MAX);
        if (ret == 0) {
            continue;
        }

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

std::vector<std::string> InOutSerialWorker::SendATCommand(const char *cmd, size_t rxCount, uint32_t timeout) {

    return atParser->SendCommand(cmd, rxCount, timeout);
}

ssize_t InOutSerialWorker::SendFrame(int channel, const unsigned char *input, int length, unsigned char type) {
    return muxParser->SendMuxFrame(channel, input, length, type);
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

