
/*
 * @file ATParser.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_ATPARSER_HPP
#define PUREPHONE_ATPARSER_HPP

#include <memory>
#include <optional>
#include <vector>
#include "FreeRTOS.h"
#include "task.h"
#include "mutex.hpp"
#include "Service/Service.hpp"

namespace bsp{
    class Cellular;
}

class ATParser {
public:

    enum class Urc{
        MeInitializationSuccessful,
        FullFuncionalityAvailable,
        SimCardReady,
        SMSInitializationComplete,
        PhonebookInitializationComplete

    };

    ATParser(bsp::Cellular* cellular);

    int ProcessNewData(sys::Service *service);

    std::vector<std::string> SendCommand(const char *cmd, size_t rxCount, uint32_t timeout = 500);

    static std::vector<std::string> Tokenizer(std::string& input,uint32_t maxTokenCount,const std::string& delimiter);

private:

    std::vector<Urc> ParseURC();

    bsp::Cellular* cellular = nullptr;

    std::string responseBuffer;

    TaskHandle_t blockedTaskHandle = nullptr;
    cpp_freertos::MutexStandard mutex;

    std::vector<ATParser::Urc> urcs;

    bool isInitialized = false;

};


#endif //PUREPHONE_ATPARSER_HPP
