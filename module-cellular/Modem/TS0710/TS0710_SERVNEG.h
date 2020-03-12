/**
 * Project Untitled
 */

#ifndef _TS0710_SERVNEG_H
#define _TS0710_SERVNEG_H

#include "TS0710_types.h"

// control channel - Service Negotiation Command (SNC)

class TS0710_SERVNEG
{
  public:
    enum Service_e
    {
        data,
        voice,
        reserved1,
        reserved2
    };
    enum VoiceCodec_e
    {
        GSM0621,
        PCM64k,
        ADPCM32k,
        CodedHalfRate,
        PCM128k,
        reserved
    };
    struct ServiceParameters_t
    {
        Service_e Service;
        VoiceCodec_e VoiceCodec;
    };

    TS0710_SERVNEG()
    {}
    ~TS0710_SERVNEG()
    {}

  private:
    /**
     * @param DLC
     * @param Service_parameters
     */
    void request(DLCI_t DLC, ServiceParameters_t Service_parameters);

    /**
     * @param DLC
     * @param Service_parameters
     */
    void indication(DLCI_t DLC, ServiceParameters_t Service_parameters);

    /**
     * @param DLC
     * @param Service_parameters
     * @param accept
     */
    void response(DLCI_t DLC, ServiceParameters_t Service_parameters, bool accept);

    /**
     * @param DLC
     * @param Service_parameters
     * @param accept
     */
    void confirm(DLCI_t DLC, ServiceParameters_t Service_parameters, bool accept);
};

#endif //_TS0710_SERVNEG_H