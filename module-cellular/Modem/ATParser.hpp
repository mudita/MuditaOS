
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

#include "ATCommon.hpp"
#include <FreeRTOS.h>
#include <Service/Service.hpp>
#include <memory>
#include <optional>
#include <task.h>
#include <vector>

namespace bsp
{
    class Cellular;
}

class ATParser : public at::Chanel
{
  public:
    enum class Urc
    {
        MeInitializationSuccessful,
        FullFuncionalityAvailable,
        SimCardReady,
        SMSInitializationComplete,
        PhonebookInitializationComplete,
        NotHandled,
    };

    ATParser(bsp::Cellular *cellular);
    virtual ~ATParser() = default;

    int ProcessNewData(sys::Service *service);

    virtual void cmd_init() override final;
    virtual void cmd_send(std::string cmd) override final;
    virtual std::vector<std::string> cmd_receive() override final;
    virtual void cmd_post() override final;

  private:
    std::vector<Urc> ParseURC();
    bsp::Cellular *cellular = nullptr;
    std::string responseBuffer;
    std::vector<ATParser::Urc> urcs;
    bool isInitialized = false;
};

#endif // PUREPHONE_ATPARSER_HPP
