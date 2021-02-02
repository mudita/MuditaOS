// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_ATPARSER_HPP
#define PUREPHONE_ATPARSER_HPP

#include "ATCommon.hpp"
#include <FreeRTOS.h>
#include <Service/Service.hpp>
#include <memory>
#include <optional>
#include <task.h>
#include <vector>
#include <message_buffer.h>

#include <bsp/cellular/CellularResult.hpp>

namespace bsp
{
    class Cellular;
}

class ATParser : public at::Channel
{
  public:
    enum class Urc
    {
        MeInitializationSuccessful,
        FullFuncionalityAvailable,
        SimCardReady,
        SMSInitializationComplete,
        PhonebookInitializationComplete,
        Fota,
        NotHandled,
    };

    ATParser(bsp::Cellular *cellular);
    virtual ~ATParser() = default;

    at::Result processNewData(sys::Service *service, const bsp::cellular::CellularResult &cellularResult);

    virtual void cmdInit() override final;
    virtual void cmdSend(std::string cmd) override final;
    virtual size_t cmdReceive(std::uint8_t *buffer, std::chrono::milliseconds timeout) override final;
    virtual void cmdPost() override final;

  private:
    std::vector<Urc> parseUrc();
    bsp::Cellular *cellular              = nullptr;
    MessageBufferHandle_t responseBuffer = nullptr;
    std::string urcBuffer                = {};
    std::vector<ATParser::Urc> urcs      = {};
};

#endif // PUREPHONE_ATPARSER_HPP
