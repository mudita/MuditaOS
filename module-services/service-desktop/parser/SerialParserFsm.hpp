#pragma once

#include <list>
#include <string>
#include "fsm/tinyfsm.hpp"


// ----------------------------------------------------------------------------
// Event declarations
//

struct MessageDataEvt               : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// SerialParserFsm (FSM base class) declaration
//

class SerialParserFsm
: public tinyfsm::Fsm<SerialParserFsm>
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

    enum MessageType : uint8_t { Invalid=34, Endpoint, RawData }; //printable characters for convenience
    static std::string              msgPayload;
    static MessageType              msgType;
    static size_t                   msgSizeBytesToRead;
    static std::string              msgPayloadSizeStr;
    static uint32_t                 msgPayloadSize;
};
