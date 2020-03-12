/*
 *  @file usermem.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 30 lip 2018
 *  @brief DYnamic memory allocator for user space
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#ifndef USERMEM_H_
#define USERMEM_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *usermalloc(size_t xWantedSize);

    void userfree(void *pv);

    size_t usermemGetFreeHeapSize(void);

    size_t usermemGetMinimumEverFreeHeapSize(void);

    void *userrealloc(void *pv, size_t xWantedSize);

#ifdef __cplusplus
}
#endif

#endif /* USERMEM_H_ */
