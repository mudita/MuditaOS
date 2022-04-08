Encapsulated cellular call processing
=====================================

This library's goal is to encapsulate the whole call flow/flows we can have.
It's end goal is to provide: actions and guards to write call state machine to asure full control over call processing.

# library organisation

Public api headers should be placed in `include/call/` catalog - these are exported in cmake to include paths for related libraries
All other headers should **not** be placed there. These are private internals of the library
