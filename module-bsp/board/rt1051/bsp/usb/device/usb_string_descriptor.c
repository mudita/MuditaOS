#include <string.h>
#include "usb_string_descriptor.h"
#include "usb.h"
#include "usb_spec.h"
#include "usb_misc.h"
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t USB_StringDescriptorBuffer[128];

const char* USB_STRING_VALUE[] = {
    USB_STRINGS(VALUE)
};

static const char* usb_string(uint8_t id)
{
    if (id > USB_STRING_MAX_ID)
        return NULL;
    return USB_STRING_VALUE[id];
}

static uint16_t languages_descriptor(uint8_t *ptr)
{
    *ptr++ = 4;
    *ptr++ = USB_DESCRIPTOR_TYPE_STRING;
    *ptr++ = 0x09;
    *ptr++ = 0x04;
    return 4;
}

static uint16_t string_descriptor(uint8_t *ptr, const char *ascii)
{
    uint16_t *utf16;
    uint8_t len = strlen(ascii);
    uint8_t total = 2 + 2*len;
    int i;
    *ptr++ = total;
    *ptr++ = USB_DESCRIPTOR_TYPE_STRING;
    utf16 = (uint16_t*)ptr;
    for(i = 0; i < len; i++)
        *utf16++ = (uint16_t)ascii[i];

    return total;
}

uint16_t USB_StringDescriptor(void* buffer, uint8_t id)
{
    uint8_t* ptr = buffer;
    const char *ascii;
    if (id == 0)
        return languages_descriptor(ptr);
    ascii = usb_string(id - 1);
    if (ascii) {
        return string_descriptor(ptr, ascii);
    }
    return 0;
}
