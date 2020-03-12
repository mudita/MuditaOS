#pragma once

#include "tinyfsm/include/tinyfsm.hpp"
#include "EndpointFsm.hpp"
#include "ParserFsm.hpp"
#include "RawDataFsm.hpp"

using fsm_list = tinyfsm::FsmList<ParserFsm, EndpointFsm, RawDataFsm>;

/** dispatch event to all FSMs */
template <typename E> void send_event(E const &event)
{
    fsm_list::template dispatch<E>(event);
}
