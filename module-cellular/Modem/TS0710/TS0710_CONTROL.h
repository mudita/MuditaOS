/**
 * Project Untitled
 * @brief The request primitive is used to convey control information to the remote port. The indication is sent to
 * the other port emulation entity. The remote port emulation entity replies with a response which is sent to the originating
 * 07.10 entity. A confirm is sent back to the port emulation entity.
 */


#ifndef _TS0710_CONTROL_H
#define _TS0710_CONTROL_H

#include "TS0710_PARNEG.h"
#include "TS0710_SERVNEG.h"
#include "TS0710_PORTNEG.h"
#include "TS0710_CONTROL.h"

//control channel - Modem Status Command (MSC)

class TS0710_CONTROL {
private:
    TS0710_PARNEG *parneg;          //control channel - DLC parameter negotiation (PN)
    TS0710_SERVNEG *servneg;        //control channel - Service Negotiation Command (SNC)
    TS0710_PORTNEG *portneg;        //control channel - Remote Port Negotiation Command (RPN)
                                    //control channel - Remote Line Status Command(RLS)
    TS0710_CONTROL *control;        //control channel - Modem Status Command (MSC)

public: 
    enum ModemSignal_e { DTR_DSR, RTS_CTS, RI, DCD };
    enum Buffers_e { DNDiscard, Discard };
    enum BreakSigSeq_e { ASAP, INSEQ };

    struct ControlParameters_t {
        ModemSignal_e ModemSignal;  //!< Modem Signal [DTR/DSR | RTS/CTS | RI | DCD ]
        int BreakSignal;            //!< Break Signal [0—3 s in steps of 200 ms, default 0ms ]
        Buffers_e Buffers;          //!< Buffers [do not discard buffers, discard buffer default: do not discard buffers]
        BreakSigSeq_e BreakSigSeq;  //!< Break signal sequence [ as soon as possible | in sequence, default: in sequence]
    };

TS0710_CONTROL() {}
~TS0710_CONTROL() {}

private:
/**
 * @param DLC
 * @param Control_parameters
 */
void request(DLCI_t DLC, ControlParameters_t Control_parameters);
    
/**
 * @param DLC
 * @param Control_parameters
 */
void indication(DLCI_t DLC, ControlParameters_t Control_parameters);
    
/**
 * @param DLC
 * @param Control_parameters
 */
void response(DLCI_t DLC, ControlParameters_t Control_parameters);
    
/**
 * @param DLC
 * @param Control_parameters
 */
void confirm(DLCI_t DLC, ControlParameters_t Control_parameters);
};

#endif //_TS0710_CONTROL_H