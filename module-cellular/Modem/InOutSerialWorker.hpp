
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

#include <optional>
#include "FreeRTOS.h"
#include "task.h"
#include "GSM0710.hpp"
#include "mutex.hpp"

class MuxDaemon;
class MuxParser;
class ATParser;

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

    InOutSerialWorker(MuxDaemon* mux);
    ~InOutSerialWorker();

    static std::optional<std::unique_ptr<InOutSerialWorker>> Create(MuxDaemon* mux);


    void SwitchMode(const Mode newMode);

    int SendATCommand(const char *cmd, uint32_t timeout);

    ssize_t SendMuxFrame(int channel,
                          const unsigned char *input,
                          int length,
                          unsigned char type);

private:

    friend void workerTaskFunction( void* ptr );

        /*
    * Purpose:  Compares two strings.
    *                strstr might not work because WebBox sends garbage before the first OK
    *                when it's not needed anymore
    * Input:      haystack - string to check
    *                length - length of string to check
    *                needle - reference string to compare to. must be null-terminated.
    * Return:    1 if comparison was success, else 0
    */
    int memstr(
            const char *haystack,
            int length,
            const char *needle);

    // Pointer to muxDaemon which is owner of this worker
    MuxDaemon* muxDaemon;

    // BSP cellular instance
    std::unique_ptr<bsp::Cellular> cellular;

    cpp_freertos::MutexStandard serOutMutex;

    std::unique_ptr<MuxParser> muxParser;
    std::unique_ptr<ATParser> atParser;


    // Write data to output buffers
    ssize_t WriteData(unsigned char *input, size_t length);


    //worker's task handle
    xTaskHandle taskHandle=nullptr;
    const uint32_t taskPriority = 0;


    Mode mode = Mode::AT;

    bool isInitialized = false;

};


#endif //PUREPHONE_INOUTSERIALWORKER_HPP
