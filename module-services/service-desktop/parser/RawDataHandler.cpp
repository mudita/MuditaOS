#include "log/log.hpp"
#include "RawDataHandler.hpp"


// ----------------------------------------------------------------------------
// Raw Data Handler states
//

class StateRawDataType
: public RawDataHandler
{
  void react(RawDataEvt const &) override {
    LOG_DEBUG("RawDataHandler: StateRawDataType");
    type = 1;
  };
};

class StateRawDataPayload
: public RawDataHandler
{
  void react(RawDataEvt const &) override {
    LOG_DEBUG("RawDataHandler: StateRawDataPayload");
    payload = 1;
  };
};


// ----------------------------------------------------------------------------
// Base State: default implementations
//

void RawDataHandler::react(RawDataEvt const &) {
  LOG_DEBUG("RawDataEvt ignored");
}

void RawDataHandler::react(RawDataMallocEvt const &) {
  LOG_DEBUG("RawDataMallocEvt ignored");
}


int RawDataHandler::type{0};
int RawDataHandler::payload{0};



// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(RawDataHandler, StateRawDataType)
