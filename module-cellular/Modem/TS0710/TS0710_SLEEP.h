/**
 * Project Untitled
 * @brief The request primitive is used to advice the receiving device that the transmitter wishes to enter a low
 * power state. The TS 0710 layer of the receiving unit sends an indication primitive to the upper layer in order to inform
 * that the transmitting unit has entered the power saving state. The TS 0710 layer will automatically transmit an
 * acknowledge message to the transmitting device, thus no response primitive is required. The confirm primitive is sent to
 * the upper layer of the transmitting device when the low power request has been received, and indicates that the TS 0710
 * layer has entered the low power mode. Note that the Receiving device is not required to enter a low power mode, but it
 * will be considered to have done so by the TS 07.10 layer.
 */


#ifndef _TS0710_SLEEP_H
#define _TS0710_SLEEP_H

class TS0710_SLEEP {
public: 

TS0710_SLEEP() {}
~TS0710_SLEEP() {}

private:
void request();
    
void indication();
    
void confirm();
};

#endif //_TS0710_SLEEP_H