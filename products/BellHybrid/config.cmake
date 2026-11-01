set(CONFIG_SIMULATOR_DISPLAY_RES_X 600)
set(CONFIG_SIMULATOR_DISPLAY_RES_Y 480)

# See PurePhone/config.cmake for full rationale. BellHybrid does not
# enable USB_DEVICE_CONFIG_MTP, so toggling this affects only CDC.
option(ENABLE_USB_TASK "Enables usage of USB Task" OFF)
