/**
 * Project Untitled
 */


#ifndef _TS0710_FLOW_H
#define _TS0710_FLOW_H

#include "TS0710.h"
//control channel

class TS0710_FLOW {
public: 
    enum State_e { enable, disable };

TS0710_FLOW() {}
~TS0710_FLOW() {}

private:
/**
 * @param DLCI
 * @param State
 */
void request(DLCI_t DLCI, State_e State);
    
/**
 * @param DLCI
 * @param State
 */
void indication(DLCI_t DLCI, State_e State);
};

#endif //_TS0710_FLOW_H