set(CONFIG_SIMULATOR_DISPLAY_RES_X 480)
set(CONFIG_SIMULATOR_DISPLAY_RES_Y 600)

# When ON, the NXP USB device stack processes USB notifications in a
# dedicated FreeRTOS task instead of directly from the USB ISR. This
# trims the time spent in IRQ context and avoids running CDC/MTP class
# logic at interrupt priority. Kept OFF by default until bench-validated
# on target hardware; the OFF path remains the production-default ISR
# routing. See third-party/usb_stack/composite.c::USB_DeviceTask.
option(ENABLE_USB_TASK "Enables usage of USB Task" OFF)
