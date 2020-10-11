#ifndef _MTP_RESPONDER_H
#define _MTP_RESPONDER_H

/* TODO: There is a need to define library configuration interface
 *       taking into account exposed values, data, etc.  */
#define CONFIG_MTP_STORAGE_NUM 1
/* USB transport ommits session id field, check 4.7.2 chapter in MTP sepcification */
#define CONFIG_MTP_SESSION_ID 0

/* TODO: need to implement this */
#define CONFIG_MTP_OBJECTS_PER_NODE

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "defines.h"

#include "mtp_storage.h"
#include "mtp_dataset.h"


/** @brief Forward declaration. All data is prived
 *         and not exposed to application.
 */
struct mtp_responder;
typedef struct mtp_responder mtp_responder_t;

/** @brief Allocate memory for internal data storage
 *
 *  @returns handle to mtp library
 *           NULL if fail to allocate memory
 */
mtp_responder_t* mtp_responder_alloc(void);

/** @brief Initialize library by setting default values
 *  @param mtp library handle
 */
void mtp_responder_init(mtp_responder_t *mtp);

/** @brief Free memory allocated by @mtp_responder_alloc */
void mtp_responder_free(mtp_responder_t *mtp);

/** @brief Set device specific data
 *  @param mtp library handle
 *  @param info about device
 *  @return zero on success
 */
int mtp_responder_set_device_info(mtp_responder_t *mtp,
                                  const mtp_device_info_t *info);

/** @brief Set storage to expose by MTP to host PC
 *  @param storage_id identifier composed by 16-bit disk identifier
 *                    and 16bit logical partition id
 *  @param api interface functions to act with filesystem on the device
 *  @param api_arg pointer to external interface's data passed to each
 *                 api call
 *  @return zero on success
 */
int mtp_responder_set_storage(mtp_responder_t *mtp,
        uint32_t disk,
        const struct mtp_storage_api *api,
        void *api_arg);

/** @brief Setup space for store data coming from handled request
 *  @param buffer pinter to memory to store data
 *  @param size of buffer
 *  */
void mtp_responder_set_data_buffer(mtp_responder_t *mtp, void *buffer, size_t size);


void mtp_responder_handle_event(mtp_responder_t *mtp, void *event);

/** @brief Handle data incoming from the host
 *  @param request buffer with incoming data
 *  @param req_size the number of bytes in request
 *  @param data_out buffer to store outgoing data (it's not response)
 *  @param out_size send data frame priror to response frame if grater than zero
 *  @returns status/error code to be send in response
 *  */
uint16_t mtp_responder_handle_request(mtp_responder_t *mtp,
        const void *request,
        const size_t req_size);

/** @brief Function returns amount of data in buffer. In case if data
 *         didn't fit into a buffer it fills it with next chunk of data
 *         and returns new chunk length.
 *  @returns amount of data in buffer, if returned value < buffer size,
 *           it means there is no more data to fetch.
 *  */
size_t mtp_responder_get_data(mtp_responder_t *mtp);

/** @brief Tells if incoming frame is container with header or just data that
 *         didn't fit in previous frames
 *  @param mtp library handle
 *  @returns true when incoming frame is only data with no header,
 *           false when it is new container
 *  */
bool mtp_responder_data_transaction_open(mtp_responder_t *mtp);

/** @brief Returns amount of data to be received in current transaction
 *  @param mtp library handle
 *  @param incoming data to be written
 *  @param size of incoming buffer
 *  @returns zero if more data in transaction is required
 *           or MTP status code
 */
uint16_t mtp_responder_set_data(mtp_responder_t *mtp, void *incoming, size_t size);

/** @brief Create a response frame according to provided error code
 *  @param library handle
 *  @param code MTP error/success code
 *  @param data_out buffer to store the frame
 *  @param size frame length to be send
 */
void mtp_responder_get_response(mtp_responder_t *mtp, uint16_t code, void *data_out, size_t *size);

void mtp_responder_transaction_reset(mtp_responder_t *mtp);


#endif /* _MTP_RESPONDER_H */

