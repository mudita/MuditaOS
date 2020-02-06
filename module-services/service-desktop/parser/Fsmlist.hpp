#pragma once

#include "fsm/tinyfsm.hpp"

#include "SerialParser.hpp"
#include "EndpointHandler.hpp"
#include "RawDataHandler.hpp"


using fsm_list = tinyfsm::FsmList<SerialParser, EndpointHandler, RawDataHandler>;

/** dispatch event to all FSMs */
template<typename E>
void send_event(E const & event)
{
  fsm_list::template dispatch<E>(event);
}
