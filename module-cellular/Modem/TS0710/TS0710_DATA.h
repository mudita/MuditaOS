/**
 * Project Untitled
 * @brief The transmitting unit initiates transmission of data using the frame type specified for the chosen DLCI by
 * means of the request primitive. The transmitted data is delivered to the upper layer of the receiving by the indication
 * primitive. No confirmation primitive exists even for the error recovery mode. In this mode TS 0710 will take care of all
 * mechanisms involved in the error checking and thus deliver data error free.
 */


#ifndef _TS0710_DATA_H
#define _TS0710_DATA_H

#include <vector>
#include "TS0710.h"

class TS0710_DATA {
public: 

TS0710_DATA(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t sysParams, std::vector<uint8_t> User_data);
~TS0710_DATA() {}

private:
/**
 * @param DLCI
 * @param User_data
 */
void request(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t sysParams, std::vector<uint8_t> User_data);
    
/**
 * @param DLCI
 * @param User_data
 * 
 */
void indication(DLCI_t DLCI, std::vector<uint8_t> User_data);

std::unique_ptr<bsp::Cellular> pv_cellular;
};

#endif //_TS0710_DATA_H