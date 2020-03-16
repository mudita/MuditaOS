/**
 * Project Untitled
 * @brief The request primitive is used by the upper layer in the transmitting device to initiate close down of the
 * selected DLC in TS 0710. The TS 0710 layer of the receiving device uses the indication primitive to inform the upper
 * layer that the DLC has been closed down.
 */

#ifndef _TS0710_DLC_RELEASE_H
#define _TS0710_DLC_RELEASE_H

#include "TS0710.h"

class TS0710_DLC_RELEASE
{
  public:
    TS0710_DLC_RELEASE(DLCI_t DLCI)
    {
        request(DLCI);
    };
    ~TS0710_DLC_RELEASE()
    {}

  private:
    /**
     * @param DLCI
     */
    void request(DLCI_t DLCI);

    /**
     * @param DLCI
     */
    void indication(DLCI_t DLCI);
};

#endif //_TS0710_DLC_RELEASE_H