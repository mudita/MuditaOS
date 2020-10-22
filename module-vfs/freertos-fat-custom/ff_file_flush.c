// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ff_headers.h"
#include "ff_file_flush.h"

#if (ffconfigUNICODE_UTF16_SUPPORT != 0)
#include <wchar.h>
#endif

FF_Error_t FF_Flush(FF_FILE *pxFile)
{

    FF_FILE *pxFileChain;
    FF_DirEnt_t xOriginalEntry;
    FF_Error_t xError;

    /* Opening a do {} while( 0 )
     * loop to allow the use of the break statement. */
    do {
        if (pxFile == NULL) {
            xError = (FF_Error_t)(FF_ERR_NULL_POINTER | FF_CLOSE);
            break;
        }
        /* It is important to check that user doesn't supply invalid
           handle or a handle invalid because of "media removed" */
        xError = FF_CheckValid(pxFile);
#if (ffconfigREMOVABLE_MEDIA != 0)
        if (FF_GETERROR(xError) == FF_ERR_FILE_MEDIA_REMOVED) {
            FF_PendSemaphore(pxFile->pxIOManager->pvSemaphore);
            pxFileChain = (FF_FILE *)pxFile->pxIOManager->FirstFile;
            if (pxFileChain == pxFile) {
                pxFile->pxIOManager->FirstFile = pxFile->pxNext;
            }
            else {
                while (pxFileChain) {
                    if (pxFileChain->pxNext == pxFile) {
                        pxFileChain->pxNext = pxFile->pxNext;
                        break;
                    }
                    pxFileChain = pxFileChain->pxNext; /* Forgot this one */
                }
            }
            FF_ReleaseSemaphore(pxFile->pxIOManager->pvSemaphore);
#if (ffconfigOPTIMISE_UNALIGNED_ACCESS != 0)
            ffconfigFREE(pxFile->pucBuffer);
#endif                            /* ffconfigOPTIMISE_UNALIGNED_ACCESS */
            ffconfigFREE(pxFile); /* So at least we have freed the pointer. */
            xError = FF_ERR_NONE;
            break;
        }
#endif /* ffconfigREMOVABLE_MEDIA */

        if (FF_isERR(xError)) {
            /* FF_ERR_FILE_BAD_HANDLE or FF_ERR_NULL_POINTER */
            break;
        }

        /* So here we have a normal valid file handle. */
        if (((pxFile->ulValidFlags & FF_VALID_FLAG_DELETED) == 0) &&
            ((pxFile->ucMode & (FF_MODE_WRITE | FF_MODE_APPEND | FF_MODE_CREATE)) != 0)) {
            /* Get the directory entry and update it to show the new file size */
            if (FF_isERR(xError) == pdFALSE) {
                xError = FF_GetEntry(pxFile->pxIOManager, pxFile->usDirEntry, pxFile->ulDirCluster, &xOriginalEntry);

                /* Now update the directory entry */
                if ((FF_isERR(xError) == pdFALSE) &&
                    ((pxFile->ulFileSize != xOriginalEntry.ulFileSize) || (pxFile->ulFileSize == 0UL))) {
                    if (pxFile->ulFileSize == 0UL) {
                        xOriginalEntry.ulObjectCluster = 0;
                    }

                    xOriginalEntry.ulFileSize = pxFile->ulFileSize;
                    xError                    = FF_PutEntry(
                        pxFile->pxIOManager, pxFile->usDirEntry, pxFile->ulDirCluster, &xOriginalEntry, NULL);
                }
            }
        }
#if (ffconfigOPTIMISE_UNALIGNED_ACCESS != 0)
        {
            if (pxFile->pucBuffer != NULL) {
                /* Ensure any unaligned points are pushed to the disk! */
                if (pxFile->ucState & FF_BUFSTATE_WRITTEN) {
                    FF_Error_t xTempError;

                    xTempError = FF_BlockWrite(pxFile->pxIOManager, FF_FileLBA(pxFile), 1, pxFile->pucBuffer, pdFALSE);
                    if (FF_isERR(xError) == pdFALSE) {
                        xError = xTempError;
                    }
                }
                pxFile->ucState = FF_BUFSTATE_INVALID;
            }
        }
#endif
        if (FF_isERR(xError) == pdFALSE) {
            xError = FF_FlushCache(pxFile->pxIOManager); /* Ensure all modified blocks are flushed to disk! */
        }
    } while (pdFALSE);
    return xError;
}
