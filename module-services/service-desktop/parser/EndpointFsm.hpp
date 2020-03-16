#pragma once

#include "ParserUtils.hpp"
extern "C"
{
#include "FreeRTOS.h"
#include "queue.h"
}

#include <stdint.h>
#include <string>
#include "tinyfsm/include/tinyfsm.hpp"
#include "json/json11.hpp"
#include "Service/Service.hpp"
// ----------------------------------------------------------------------------
// Event declarations
//

struct EndpointEvt : tinyfsm::Event
{
    EndpointEvt(sys::Service *ownerServicePtr) : ownerService(ownerServicePtr){};
    sys::Service *ownerService;
};

// ----------------------------------------------------------------------------
// EndpointFsm (FSM base class) declaration
//
class EndpointFsm : public tinyfsm::Fsm<EndpointFsm>
{
    friend class Fsm;

  public:
    static xQueueHandle sendQueue;

  private:
    void react(tinyfsm::Event const &){};
    virtual void react(EndpointEvt const &);
    virtual void entry(void){};
    void exit(void){};

  protected:
    bool putToSendQueue(std::string);
    static sys::Service *ownerService;
    static parserutils::Endpoint endpoint;
    static uint8_t method;
    static uint32_t uuid;
    static json11::Json body;
};
