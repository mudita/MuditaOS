#pragma once

#include <list>
#include <string>
#include "fsm/tinyfsm.hpp"


// ----------------------------------------------------------------------------
// Event declarations
//

struct MessageDataEvt               : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// SerialParser (FSM base class) declaration
//

class SerialParser
: public tinyfsm::Fsm<SerialParser>
{
    friend class Fsm;
    friend class StateDecodeJson;

public:
    static std::list<int> msgChunk;

protected:

    void react(tinyfsm::Event const &) { }; /* default reaction for unhandled events */
    virtual void react(MessageDataEvt const &);
    virtual void entry(void) { };  /* entry actions in some states */
    virtual void exit(void)  { };  /* entry actions in some states */

    enum MessageType : uint8_t { invalid=34, endpoint, rawdata }; //printable characters for convenience
    static std::string              msgPayload;
    static MessageType              msgType;
    static size_t                   msgSizeBytesToRead;
    static std::string              msgPayloadSizeStr;
    static uint32_t                 msgPayloadSize;
};
