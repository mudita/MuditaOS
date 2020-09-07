#ifndef _USB_STRING_DESCRIPTOR_H
#define _USB_STRING_DESCRIPTOR_H

#include <stdint.h>

/* Macros for build USB string database in "usb_strings.h" file,
 * that is placed somewhere in application layer. Header doesn't
 * need to have guard defines as it's only included here.
 *
 * Example usage (content of usb_string.h):
 *
 *  #define USB_STRINGS(entry) \
        entry(MANUFACTURER, "Fancy manufacturer"), \
        entry(DESCRIPTION, "Fancy device model")
 *
 * For enum, there is prefix added: USB_STRING_.
 */
#include "usb_strings.h"
#define ID(id, value) USB_STRING_##id
#define VALUE(id,value) value

typedef enum {
    USB_STRING_EMPTY = 0,
    USB_STRINGS(ID),
    USB_STRING_MAX_ID
} usb_device_string_id;

/*
 * @brief Returns USB string descriptor for id
 * @param buffer for descritor's binary data
 * @param id of string to be put in descritor
 * @return length of descriptor's binary data or 0
 *         for non-existing string
 */
uint16_t USB_StringDescriptor(void* buffer, uint8_t id);

/**/
extern uint8_t USB_StringDescriptorBuffer[];
#endif /*_USB_STRING_DESCRIPTOR_H */

