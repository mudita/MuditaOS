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
  /* NOTE: react(), entry() and exit() functions need to be accessible
   * from tinyfsm::Fsm class. You might as well declare friendship to
   * tinyfsm::Fsm, and make these functions private:
   *
   * friend class Fsm;
   */
public:

    static uint8_t linux_fd;

    /* default reaction for unhandled events */
  void react(tinyfsm::Event const &) { };

 /* pure virtual: enforce implementation in all states */
  virtual void react(EndpointEvt   const &);


  virtual void entry(void) { };           /* no entry actions at all */
  void exit(void)  { };                   /* no exit actions at all */

protected:

  enum Endpoint: uint8_t { Battery, Backups, DeviceInfo, Network, Storage };
  static uint8_t  endpoint;
  static uint8_t  method;

  std::string rspPayloadSizeToStr(std::size_t);
};
