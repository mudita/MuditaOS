/**
 * Project Untitled
 * @brief When the request primitive is passed to the TS 0710 layer of the transmitting device close down of the
 * multiplexer mode is initiated and a close down command is sent to the receiving device. On reception of the close down
 * command the TS 0710 layer of the receiving device sends the indication primitive to the upper layer and the multiplexer
 * mode is terminated.
 */


#ifndef _TS0710_CLOSE_H
#define _TS0710_CLOSE_H

//control channel - Multiplexer close down (CLD)
class TS0710_CLOSE {
public: 
TS0710_CLOSE() {}
~TS0710_CLOSE() {}

private:
void request();
    
void indication();
};

#endif //_TS0710_CLOSE_H