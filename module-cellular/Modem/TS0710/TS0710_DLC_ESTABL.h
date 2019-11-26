/**
 * Project Untitled
 */


#ifndef _TS0710_DLC_ESTABL_H
#define _TS0710_DLC_ESTABL_H

#include "TS0710.h"

class TS0710_DLC_ESTABL {
public: 
    //DLC_ESTABL_SystemParameters passed to Port Negotiation frame in control channel

private:
    DLC_ESTABL_SystemParameters_t pv_SystemParameters;
    DLCI_t pv_DLCI;
    bool active = false;
    bsp::Cellular *pv_cellular;

public:
    TS0710_DLC_ESTABL(DLCI_t DLCI, bsp::Cellular *cellular);
    TS0710_DLC_ESTABL(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters, bsp::Cellular *cellular) { pv_DLCI = DLCI; pv_SystemParameters = system_parameters; pv_cellular = cellular; active = request(DLCI, system_parameters); };
    ~TS0710_DLC_ESTABL() {}

    bool getResponse() { return active; }
    DLC_ESTABL_SystemParameters_t getParams() { return pv_SystemParameters; }

private:
/**
 * @param DLCI
 * @param system_parameters
 * @brief The transmitting device uses the request primitive initiate the establishment of a new DLC with a desired set of system parameters on the multiplexer channel.
 */
bool request(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters);
    
/**
 * @param DLCI
 * @param system_parameters
 * @brief The indication primitive is passed to the upper layer by the TS 0710 layer of the receiving device on reception of the DLC establishment request.
 */
void indication(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters);
    
/**
 * @param DLCI
 * @param system_parameters
 * @return accept
 * @brief The receiving device uses the response primitive to either accept or reject the proposed DLCI with its system parameters.
 */
bool response(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters);
    
/**
 * @param DLCI
 * @param system_parameters
 * @return accept
 * @brief The confirm primitive is passed to the upper layer of the transmitting device on reception of the response from the receiving device.
 */
bool confirm(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters);
};

#endif //_TS0710_DLC_ESTABL_H