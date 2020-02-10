#include "log/log.hpp"
#include "RawDataFsm.hpp"


// ----------------------------------------------------------------------------
// Raw Data FSM states
//

class StateRawDataType
: public RawDataFsm
{
  void react(RawDataEvt const &) override {
    LOG_DEBUG("RawDataFsm: StateRawDataType");
    type = 1;
  };
};

class StateRawDataPayload
: public RawDataFsm
{
  void react(RawDataEvt const &) override {
    LOG_DEBUG("RawDataFsm: StateRawDataPayload");
    payload = 1;
  };
};


// ----------------------------------------------------------------------------
// Base State: default implementations
//

void RawDataFsm::react(RawDataEvt const &) {
  LOG_DEBUG("RawDataEvt ignored");
}

void RawDataFsm::react(RawDataMallocEvt const &) {
  LOG_DEBUG("RawDataMallocEvt ignored");
}


int RawDataFsm::type{0};
int RawDataFsm::payload{0};



// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(RawDataFsm, StateRawDataType)
