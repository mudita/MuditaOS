Configs description
====================

# ProjectConfig.cmake

| LOG_USE_COLOR | Result |
|---:|---:|
| 0 | No color on RTT log messages |
| 1 | Color in RTT log, SysView is not parsing it | 

| LOG_REDIRECT | Result |
|---:|---:|
| 0 | RTT log on serial (default read with JLinkRTTClient) |
| 1 | RTT log on usb device | 
| 2 | RTT log on SystemView | 

| SYSTEM_VIEW_ENABLED | Result |
|---:|---:|
| 0 | SystemView not enabled and code not included|
| 1 | SystemView enabled and code included | 
