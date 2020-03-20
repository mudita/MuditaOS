#include "segger/log/log.hpp"
#include "RawDataFsm.hpp"

// ----------------------------------------------------------------------------
// Raw Data FSM states
//

class StateRawDataType : public RawDataFsm
{
    void react(RawDataEvt const &) override
    {
        LOG_DEBUG("RawDataFsm: StateRawDataType");
    };
};

class StateRawDataPayload : public RawDataFsm
{
    void react(RawDataEvt const &) override
    {
        LOG_DEBUG("RawDataFsm: StateRawDataPayload");
    };
};

// ----------------------------------------------------------------------------
// Base State: default implementations
//

void RawDataFsm::react(RawDataEvt const &)
{
    LOG_DEBUG("RawDataEvt ignored");
}

void RawDataFsm::react(RawDataMallocEvt const &evt)
{
    LOG_DEBUG("RawDataMallocEvt ignored");
}

int RawDataFsm::type{0};
int RawDataFsm::payload{0};

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(RawDataFsm, StateRawDataType)
