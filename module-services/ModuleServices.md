Module services
===============

All Services in PurePhone are dependent to `Service.hpp` from `module-sys`

Table of Contents

* [Module services](#module-services)
* [General services documentation](#general-services-documentation)
* [How to add new service](#how-to-add-new-service)
   * [To change service startup order](#to-change-service-startup-order)
   * [To use settings::Settings](#To-use-settingssettings)
   * [To add timer](#to-add-timer)
   * [To add 3rd party library](#to-add-3rd-party-library)
   * [To add tests](#to-add-tests)
* [More on services configuration](#more-on-services-configuration)
* [Minimalistic service example](#Example)

# General services documentation

Services documentation is available [in module-sys here](/module-sys/README.md#Services).

# How to add new service

- we do not have strict naming conventions, but please follow others
- name your service - for example’s sake it will be `test`
- add new folder `service-test` in `module-services`
- add folder `include/service-test/` to `service-test`
    - this folder will contain your service public API - api you want to publish for others to use
    - add this folder as `PUBLIC` library include
    - please do add other elements as `PRIVATE` library includes and libraries
- create `include/service-test/ServiceTest.hpp` - this file will be your service declaration
- create `ServiceTest.cpp` -  this file will be your service entry point and definition
- add your service to SERVICES list in `module-services/CMakeLists.txt`
- add your service to list of services to include in product main in which you want to have it
    - add your service `$<$<BOOL:${ENABLE_SERVICE_TEST}>:service-test>` in `target_link_libraries(${TARGET} ...`
    - please add conditional compilation to it too in `${TARGET}Main.cpp` (i.e. `PurePhoneMain.cpp`) - all services have `ENABLE_SERVICE_{SERVICE_NAME}` definition exported

**NOTE:** There are no other rules of thumb in terms of folders

- add cmake, the very basic cmake here:
``` cmake
project(service-test)
message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES ServiceTest.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    PUBLIC
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
```
- to disable your service by default please add:
```
option(ENABLE_SERVICE_TEST "Enable service test" OFF)
```

## To change service startup order

Please see the [SystemManager services synchronization](/module-sys/SystemManager/doc/ServicesSynchronization.md) documentation

## To use settings::Settings

Documentation: [settings::Settings](./service-db/doc/Settings.md)

## To add timer

Documentation [timers](/module-sys/README.md#Timers)

## To add 3rd party library

Documentation: [third party libraries](/third-party/ThirdParty.md)

## To add tests

Documentation: [unit tests gathering cmake](/test/AddingUnitTests.md)

## To change service order, failure timeout or name

Documentation [service manifest](/module-sys/SystemManager/doc/ServicesSynchronization.md)

# More on services configuration

While we can enable and disable services, we do not have proper separation for services and API definition.
This means that whole system compilation will depend on:
- include paths exported in services
- APIs from services

# Example

See [service-test example](./service-test/doc/README.md) for ready to copy service example
