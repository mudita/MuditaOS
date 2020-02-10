#pragma once

#include "fsm/tinyfsm.hpp"


// ----------------------------------------------------------------------------
// Event declarations
//

struct RawDataEvt           : tinyfsm::Event { };
struct RawDataMallocEvt     : tinyfsm::Event { };

// ----------------------------------------------------------------------------
// RawDataFsm (FSM base class) declaration
//
class RawDataFsm
: public tinyfsm::Fsm<RawDataFsm>
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

 
    virtual void react(RawDataEvt   const &); /* pure virtual: enforce implementation in all states */
    void react(RawDataMallocEvt   const &);

    void entry(void) { };          /* no entry actions at all */
    void exit(void)  { };          /* no exit actions at all */

protected:

    static int type;
    static int payload;
};
