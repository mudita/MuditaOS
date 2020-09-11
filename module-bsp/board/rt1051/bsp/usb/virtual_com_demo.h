#ifndef _VIRTUAL_COM_DEMO_H_
#define _VIRTUAL_COM_DEMO_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "event_groups.h"

#include "virtual_com.h"

void VirtualComDemoInit(usb_cdc_vcom_struct_t *cdcVcom);

#endif /* _VIRTUAL_COM_DEMO_H_ */
