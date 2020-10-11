#ifndef _MTP_H_
#define _MTP_H_

#include "mtp_responder.h"
#include "mtp_fs.h"

// refactor name to mtp_app_struct_t
typedef struct {
    class_handle_t classHandle;
    mtp_responder_t *responder;
    struct mtp_fs *mtp_fs;
    void *sd_disk;

    uint8_t configured;
    uint8_t in_reset;
    MessageBufferHandle_t inputBox;
    MessageBufferHandle_t outputBox;
} usb_mtp_struct_t;

usb_status_t MtpUSBCallback(uint32_t event, void *param, void *userArg);
usb_status_t MtpInit(usb_mtp_struct_t *mtpApp, class_handle_t classHandle);
void MtpDetached(usb_mtp_struct_t *mtpApp);

#endif /* _MTP_H_ */
