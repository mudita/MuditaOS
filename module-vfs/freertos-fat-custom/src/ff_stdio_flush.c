// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

/* FreeRTOS+FAT includes. */
#include "ff_headers.h"
#include "ff_stdio.h"

#if (ffconfigTIME_SUPPORT != 0)
#include <time.h>
#endif

#include "ff_file_flush.h"

int prvFFErrorToErrno(FF_Error_t xError);

int ff_fflush(FF_FILE *pxStream)
{
    FF_Error_t xError;
    int iReturn, ff_errno;

#if (ffconfigDEV_SUPPORT != 0)
    {
        /* Currently device support is in an experimental state.  It will allow
            to create virtual files. The I/O data to those files will be redirected
            to their connected "drivers". */
        if (pxStream != NULL) {
            FF_Device_Flush(pxStream);
        }
    }
#endif

    xError   = FF_Flush(pxStream);
    ff_errno = prvFFErrorToErrno(xError);

    if (ff_errno == 0) {
        iReturn = 0;
    }
    else {
        /* Return -1 for error as per normal fclose() semantics. */
        iReturn = -1;
    }

    /* Store the errno to thread local storage. */
    stdioSET_ERRNO(ff_errno);

    return iReturn;
}
