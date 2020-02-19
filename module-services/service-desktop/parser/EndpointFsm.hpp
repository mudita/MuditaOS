#pragma once

#include <stdint.h>
#include <string>
#include "fsm/tinyfsm.hpp"



// ----------------------------------------------------------------------------
// Event declarations
//

struct EndpointEvt   : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// EndpointFsm (FSM base class) declaration
//
class EndpointFsm
: public tinyfsm::Fsm<EndpointFsm>
{
    friend class Fsm;

  private:
    void react(tinyfsm::Event const &){};
    virtual void react(EndpointEvt const &);
    virtual void entry(void){};
    void exit(void){};

  protected:
    static uint8_t endpoint;
    static uint8_t method;
};
