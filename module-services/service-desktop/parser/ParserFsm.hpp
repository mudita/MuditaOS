#pragma once

#include "fsm/tinyfsm.hpp"
#include <list>
#include <string>

// ----------------------------------------------------------------------------
// Event declarations
//

struct MessageDataEvt : tinyfsm::Event
{
};

// ----------------------------------------------------------------------------
// ParserFsm (FSM base class) declaration
//

class ParserFsm : public tinyfsm::Fsm<ParserFsm>
{
    friend class Fsm;
    friend class StateDecodeJson;

  public:
    static std::list<int> msgChunk;

  protected:
    void react(tinyfsm::Event const &){}; /* default reaction for unhandled events */
    virtual void react(MessageDataEvt const &);
    virtual void entry(void){}; /* entry actions in some states */
    void exit(void){};          /* no exit actions at all */

    static std::string msgPayload;
    static uint8_t msgType;
    static size_t msgSizeBytesToRead;
    static std::string msgPayloadSizeStr;
    static uint32_t msgPayloadSize;
};
