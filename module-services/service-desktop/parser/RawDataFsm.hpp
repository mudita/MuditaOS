#pragma once

#include "tinyfsm/include/tinyfsm.hpp"

// ----------------------------------------------------------------------------
// Event declarations
//

struct RawDataEvt : tinyfsm::Event
{};
struct RawDataMallocEvt : tinyfsm::Event
{};

// ----------------------------------------------------------------------------
// RawDataFsm (FSM base class) declaration
//
class RawDataFsm : public tinyfsm::Fsm<RawDataFsm>
{
    friend class Fsm;

  private:
    void react(tinyfsm::Event const &){};   /* default reaction for unhandled events */
    virtual void react(RawDataEvt const &); /* pure virtual: enforce implementation in all states */
    void react(RawDataMallocEvt const &);
    void entry(void){}; /* no entry actions at all */
    void exit(void){};  /* no exit actions at all */

  protected:
    static int type;
    static int payload;
};
