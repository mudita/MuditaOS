// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ff_stdio_listdir_recursive.h>

const char *prvABSPath(const char *pcPath);

static int ff_stdio_listdir_recursive_prv(char *startPath, void (*callback)(void *, const char *, bool), void *context)

{
    FF_FindData_t *pxFindData;
    BaseType_t xIsDir, xIsDotDir;
    int iResult, iNext, iNameLength, pass, iCount = 0;

    pxFindData = (FF_FindData_t *)ffconfigMALLOC(sizeof(*pxFindData));
    if (pxFindData != NULL) {
        iNameLength = (int)strlen(startPath);
        for (pass = 0; pass < 2; pass++) {
            for (iResult = ff_findfirst(startPath, pxFindData); iResult == 0; iResult = iNext) {
                xIsDir = (pxFindData->xDirectoryEntry.ucAttrib & FF_FAT_ATTR_DIR) != 0;
                if ((pass == 0) && (xIsDir != pdFALSE)) {
                    /* This entry is a directory.  Don't traverse '.' or '..' */
                    xIsDotDir = 0;

                    if (pxFindData->pcFileName[0] == '.') {
                        if ((pxFindData->pcFileName[1] == '.') && (pxFindData->pcFileName[2] == '\0')) {
                            xIsDotDir = 2;
                        }
                        else if (pxFindData->pcFileName[1] == '\0') {
                            xIsDotDir = 1;
                        }
                    }
                    if (xIsDotDir == 0) {
                        snprintf(startPath + iNameLength,
                                 (size_t)(ffconfigMAX_FILENAME - iNameLength),
                                 "%s%s",
                                 startPath[iNameLength - 1] == '/' ? "" : "/",
                                 pxFindData->pcFileName);

                        /* Let pxFindData point to the next element before
                        the current will get removed. */
                        iNext = ff_findnext(pxFindData);

                        /* Remove the contents of this directory. */
                        iResult = ff_stdio_listdir_recursive_prv(startPath, callback, context);
                        if (iResult < 0) {
                            iCount = -1;
                            break;
                        }
                        iCount += iResult;
                        iCount++;
                        if (callback)
                            callback(context, startPath, true);
                    }
                    else {
                        iNext = ff_findnext(pxFindData);
                    }
                }
                else if ((pass == 1) && (xIsDir == pdFALSE)) {
                    snprintf(startPath + iNameLength,
                             (size_t)(ffconfigMAX_FILENAME - iNameLength),
                             "%s%s",
                             startPath[iNameLength - 1] == '/' ? "" : "/",
                             pxFindData->pcFileName);
                    iNext = ff_findnext(pxFindData);
                    iCount++;
                    if (callback)
                        callback(context, startPath, false);
                }
                else {
                    iNext = ff_findnext(pxFindData);
                }
                startPath[iNameLength] = '\0';
            }

            if (FF_GETERROR(iResult) == FF_ERR_DIR_INVALID_PATH) {
                break;
            }
            if ((FF_GETERROR(iResult) != FF_ERR_DIR_END_OF_DIR) && (FF_GETERROR(iResult) != FF_ERR_FILE_INVALID_PATH)) {
                FF_PRINTF("ff_listdir_recurse[%s]: %s\n", startPath, (const char *)FF_GetErrMessage(iResult));
            }
        }
        ffconfigFREE(pxFindData);
    }
    else {
        iCount = -1;
        stdioSET_ERRNO(pdFREERTOS_ERRNO_ENOMEM);
    }
    return iCount;
}

/**
 * List all directories recursive
 * @param startPath Starting directory
 * @param callback  Callback handler
 * @param context   private context
 * @return  Error code
 */
int ff_stdio_listdir_recursive(const char *startPath, void (*callback)(void *, const char *, bool), void *context)
{
    int iResult;
    char *pcPath;
    pcPath = (char *)ffconfigMALLOC(ffconfigMAX_FILENAME);
    if (pcPath != NULL) {
        /* In case a CWD is used, get the absolute path */
        startPath = prvABSPath(startPath);
        snprintf(pcPath, ffconfigMAX_FILENAME, "%s", startPath);
        /* This recursive function will do all the work */
        iResult = ff_stdio_listdir_recursive_prv(pcPath, callback, context);
        if (iResult >= 0) {
            if (callback)
                callback(context, pcPath, true);
        }
        ffconfigFREE(pcPath);
    }
    else {
        iResult = -1;
        stdioSET_ERRNO(pdFREERTOS_ERRNO_ENOMEM);
    }
    return iResult;
}
