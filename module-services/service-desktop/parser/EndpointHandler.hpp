#pragma once

#include <stdint.h>
#include "fsm/tinyfsm.hpp"



// ----------------------------------------------------------------------------
// Event declarations
//

struct EndpointEvt   : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// EndpointHandler (FSM base class) declaration
//
class EndpointHandler
: public tinyfsm::Fsm<EndpointHandler>
{
  /* NOTE: react(), entry() and exit() functions need to be accessible
   * from tinyfsm::Fsm class. You might as well declare friendship to
   * tinyfsm::Fsm, and make these functions private:
   *
   * friend class Fsm;
   */
public:

  /* default reaction for unhandled events */
  void react(tinyfsm::Event const &) { };

 /* pure virtual: enforce implementation in all states */
  virtual void react(EndpointEvt   const &);


  virtual void entry(void) { };           /* no entry actions at all */
  void exit(void)  { };                   /* no exit actions at all */

protected:

  enum Method : uint8_t { GET, POST, DELETE };
  enum Endpoint : uint8_t { BATTERY, BACKUPS, DEVICE_INFO, NETWORK, STORAGE };
  static uint8_t  endpoint;
  static uint8_t  method;
};
