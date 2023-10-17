Configs description
====================

# ProjectConfig.cmake

## Logging

### Logging device and configuration

| LOG_USE_COLOR | Result |
|---:|---:|
| 0 | No color on RTT log messages |
| 1 | Color in RTT log, SysView is not parsing it | 

| LOG_REDIRECT | Result |
|---:|---:|
| RTT_JLINK | RTT log via JLINK on serial|
| RTT_LUART | RTT log via RT1051 UART on serial| 
| RTT_SYSTEMVIEW | RTT log via JLINK on SystemView | 

| SYSTEM_VIEW_ENABLED | Result |
|---:|---:|
| 0 | SystemView not enabled and code not included|
| 1 | SystemView enabled and code included | 

### LOG_SENSITIVE_DATA

Enables logging normally disabled sensitive data

## System tracing

## SystemView option

While it's not a perfect solution due to:
- loads of data (and lost data) sent via RTT
- no scalling with CPU frequency
We can to some extent use Segger SystemView. To enable set `SYSTEMVIEW` option to `ON`

### System Profile

MuditaOS have a minimailstic system profiling tool, to enable set: `SYSTEM_PROFILE` to `ON`

System Profile data is being sent via msgpack on RTT. To decode it and see:
- last 100ms before frequency change load
- frequency changes in time
- power usage in time
As well as logs in one app you can use: [MuditaOSTop](https://github.com/mudita/MuditaOSTop)

Or change printer configuration in `CPUStatistics.cpp` to other than messagepack printer (i.e. logs)

## CurrentMeasurement enable option
To use direct current polling and have it in logs set `CURRENT_MEASUREMENT` to `ON`
you can plot this with [plot_current_measurement.py](../tools/plot_current_measurement.py)

# USB

## USB-CDC echo test

To test if USB-CDC works you can set USB-CDC echo with `USBCDC_ECHO`

## Enable/disable USB MTP

- option `ENABLE_USB_MTP` enables USB-MTP protocol, default `ON`

## Mudita USB Vendor/Product IDs

- option `MUDITA_USB_ID` default `ON`

# Config options for the lwext4

Following configurations are for: [lwext4](third-party/lwext4/) - third party ext4 library

## LWEXT4 debug options

- option `LWEXT4_DEBUG_PRINTF` default `ON`
- option `LWEXT4_DEBUG_ASSERT` default `OFF`

## LWEXT4 sectors cache size

- option `LWEXT4_CACHE_SIZE` default set to `256`

# Development Configuration

- option `WITH_DEVELOPMENT_FEATURES` enables all system development features, which are:
    - `DEVELOPER_SETTINGS_OPTIONS_DEFAULT`
    - `ENABLE_DEVELOPER_MODE_ENDPOINT_DEFAULT`
    - `LOG_SENSITIVE_DATA_ENABLED`
default: `OFF`

# Proprietary/Community build
- option `ASSETS_TYPE` default `Proprietary`

Setting to `Community` disables the download of proprietary files which are unaccessable for the community
and disables tests basing on those files
