examplary test service
======================

To enable it:
1. configure the project
2. run `ccmake .` in the build catalog and set: `ENABLE_SERVICE_TEST` to `ON`

By default, this service will:
1. add and log variable from settings::Settings
2. log from a timer each 1000ms
