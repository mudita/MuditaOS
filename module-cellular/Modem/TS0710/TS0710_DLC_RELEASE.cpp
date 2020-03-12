/**
 * Project Untitled
 */

#include "TS0710_DLC_RELEASE.h"

/**
 * TS0710_DLC_RELEASE implementation
 */

/**
 * @param DLCI
 */
void TS0710_DLC_RELEASE::request(DLCI_t DLCI)
{
    /*
    TE sends DISC frame to request for closing down DLC 1
        TE→MS  F9 07 53 01 3f F9
    MS feeds back UA frame to accept
        TE←MS  F9 07 73 01 15 F9
    */
}

/**
 * @param DLCI
 */
void TS0710_DLC_RELEASE::indication(DLCI_t DLCI)
{}