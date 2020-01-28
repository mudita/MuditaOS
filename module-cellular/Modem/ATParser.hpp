
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

#define DEBUG_MODEM_OUTPUT_RESPONSE 1
#define DEBUG_MODEM_TIMEOUT_AS_ERROR 0

#if DEBUG_MODEM_TIMEOUT_AS_ERROR
#define LOG_MODEM_TIMEOUT(...) LOG_ERROR(__VA_ARGS__)
#else
#define LOG_MODEM_TIMEOUT(...) LOG_INFO(__VA_ARGS__)
#endif

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

    // @param maxTokenCount max token count, if 0 no max number limitation
    static std::vector<std::string> Tokenizer(const std::string &input, const std::string &delimiter, uint32_t maxTokenCount = 0);

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
