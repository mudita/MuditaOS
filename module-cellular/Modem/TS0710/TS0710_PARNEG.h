/**
 * Project Untitled
 * @brief The request primitive is used to request that the remote 07.10 entity changes a specific DLC connection
 * parameters. An indication is sent to the remote port emulation entity. The remote emulation entity replies with a
 * response witch is forwarded as an confirmation to the originating port emulating entity.
 */

#ifndef _TS0710_PARNEG_H
#define _TS0710_PARNEG_H

#include "TS0710.h"
// control channel - DLC parameter negotiation (PN)

class TS0710_PARNEG
{
  public:
    struct DLCParameters_t
    {
        TypeOfFrame_e TypeOfFrame; //!< Type of frame [UIH | UI | I, default: UIH]
        int ConvergenceLayer;      //!< Convergence layer [1 - 4, default: 1]
        int Priority;              //!< Priority [0-63], default according to table in subclause 5.6
        int AckTime;               //!< Acknowledgement Timer [10 ms - 25.5 sec, deault: 100 ms]
        int MaxFrameSize; //!< Maximum Frame Size [1 – 32768, default: 31 for the basic option and 64 for the advanced
                          //!< option]
        int MaxNumOfRetransmissions; //!< Maximum number of retransmissions [0 – 100, default : 3]
        int MuxCtrlRespTime;    //!< Response timer for the multiplexor control channel [0,01s-2,55s, default: 0,3s]
        int WakeUpRespTime;     //!< Wake up response timer [1s – 255s, default 10s]
        int ErrRecovWindowSize; //!< Window size for error recovery mode [1 – 7, default : 2]
    };

    TS0710_PARNEG()
    {}
    ~TS0710_PARNEG()
    {}

  private:
    /**
     * @param DLC
     * @param DLC_parameters
     */
    void request(DLCI_t DLC, DLCParameters_t DLC_parameters);

    /**
     * @param DLC
     * @param DLC_parameters
     */
    void indication(DLCI_t DLC, DLCParameters_t DLC_parameters);

    /**
     * @param DLC
     * @param DLC_parameters
     * @param accept
     */
    void response(DLCI_t DLC, DLCParameters_t DLC_parameters, bool accept);

    /**
     * @param DLC
     * @param DLC_parameters
     * @param accept
     */
    void confirm(DLCI_t DLC, DLCParameters_t DLC_parameters, bool accept);
};

#endif //_TS0710_PARNEG_H