/**
 * Project Untitled
 * @brief The request primitive is used to request that the remote port changes its parameters. The indication is sent
 * to the other port emulation entity. The remote port emulation entity replies with a response. A confirm is sent to the
 * originating port entity.
 */


#ifndef _TS0710_PORTNEG_H
#define _TS0710_PORTNEG_H

#include "TS0710_types.h"

//control channel - Remote Port Negotiation Command (RPN)
//control channel - Remote Line Status Command(RLS)

class TS0710_PORTNEG {
public: 
    enum DataBits_e { DB5, DB6, DB7, DB8 };
    enum PortSpeed_e { PS2400, PS4800, PS7200, PS9600, PS19200, PS38400, PS57600, PS115200, PS230400 }; 
    enum Parity_e { NoParity, Parity };
    enum ParityType_e { odd, even, mark, space };
    enum StopBits_e { SB1, SB15 }; 
    enum LineStatusParameter_e { NoErrors, OverrunErr, ParityErr, FramingErr }; //!< Port speed [no errors, overrun error, parity error, framing error]

    struct PortParameters_t {
        PortSpeed_e PortSpeed;
        DataBits_e DataBits;
        StopBits_e StopBits;
        Parity_e Parity;
        ParityType_e ParityType;
    };

TS0710_PORTNEG() {}
~TS0710_PORTNEG() {}

private:
/**
 * @param DLC
 * @param Port_parameters
 */
void request(DLCI_t DLC, PortParameters_t Port_parameters);
    
/**
 * @param DLC
 * @param Port_parameters
 */
void indication(DLCI_t DLC, PortParameters_t Port_parameters);
    
/**
 * @param DLC
 * @param Port_parameters
 * @param accept
 */
void response(DLCI_t DLC, PortParameters_t Port_parameters, bool accept);
    
/**
 * @param DLC
 * @param Port_parameters
 * @param accept
 */
void confirm(DLCI_t DLC, PortParameters_t Port_parameters, bool accept);
    
/**
 * @param DLC
 * @param LineStatusParameter
 */
void request(DLCI_t DLC, LineStatusParameter_e LineStatusParameter);
    
/**
 * @param DLC
 * @param LineStatusParameter
 */
void indication(DLCI_t DLC, LineStatusParameter_e LineStatusParameter);
};

#endif //_TS0710_PORTNEG_H