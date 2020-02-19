#pragma once

#include "fsm/tinyfsm.hpp"

#include "SerialParserFsm.hpp"
#include "EndpointFsm.hpp"
#include "RawDataFsm.hpp"


using fsm_list = tinyfsm::FsmList<SerialParserFsm, EndpointFsm, RawDataFsm>;

/** dispatch event to all FSMs */
template<typename E>
void send_event(E const & event)
{
    fsm_list::template dispatch<E>(event);
}
