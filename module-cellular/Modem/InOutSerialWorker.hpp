
/*
 * @file InOutSerialWorker.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_INOUTSERIALWORKER_HPP
#define PUREPHONE_INOUTSERIALWORKER_HPP

#include "FreeRTOS.h"
#include "task.h"
#include "GSM0710.hpp"
#include "mutex.hpp"

class MuxDaemon;

namespace bsp{
    class Cellular;
}

void workerTaskFunction( void* ptr );

class InOutSerialWorker {
public:

    enum class Mode{
        AT,
        CMUX
    };

    static std::unique_ptr<InOutSerialWorker> Create(MuxDaemon* mux);

    InOutSerialWorker(MuxDaemon* mux);
    ~InOutSerialWorker();


    void SwitchMode(Mode newMode);

    // Write data to output buffers
    ssize_t WriteData(unsigned char *input, size_t length);

private:

    friend void workerTaskFunction( void* ptr );

    // Pointer to muxDaemon which is owner of this worker
    MuxDaemon* muxDaemon;

    // BSP cellular instance
    std::unique_ptr<bsp::Cellular> cellular;

    cpp_freertos::MutexStandard serOutMutex;

    int ReadIncomingData();
    int ExtractFrames();
    int HandleCtrlChannelCommands(GSM0710Frame* frame);

    //worker's task handle
    xTaskHandle taskHandle=nullptr;
    const uint32_t taskPriority = 0;

    std::unique_ptr<GSM0710Buffer> inputBuffer=nullptr;

    Mode mode = Mode::AT;

    bool isInitialized = false;

};


#endif //PUREPHONE_INOUTSERIALWORKER_HPP
