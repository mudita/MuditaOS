cmake_minimum_required(VERSION 3.14)


if(TARGET module-sys)
    return() # The project has already been built.
endif()


project(module-sys VERSION 1.0
        DESCRIPTION "Core module library")

set(CMAKE_CXX_STANDARD 14)

# Get the current working branch
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(MOD_VERSION ${PROJECT_VERSION})

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/version.h.in
        ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-version.h
)

set(SOURCES

        ${CMAKE_CURRENT_LIST_DIR}/Service/Bus.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Service/Message.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Service/Service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SystemManager/SystemManager.cpp

        )

message("PROJECT TARGET: ${PROJECT_TARGET}")
if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()


#message("${PROJECT_NAME}: add_subdirectory module-os")
#add_subdirectory(module-os)


add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


target_link_libraries(${PROJECT_NAME} PUBLIC module-os)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})



target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        CPP_FREERTOS_CONDITION_VARIABLES
        )

target_compile_options(${PROJECT_NAME}

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

target_compile_features(${PROJECT_NAME} PUBLIC

        cxx_std_14
        cxx_noexcept
)





# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

       add_executable(unittest_${PROJECT_NAME}
                ${CMAKE_CURRENT_LIST_DIR}/tests/Test1/TestService1.cpp
                ${CMAKE_CURRENT_LIST_DIR}/tests/Test1/CommandsService.cpp
                ${CMAKE_CURRENT_LIST_DIR}/tests/Test1/DelayedService.cpp

                ${CMAKE_CURRENT_LIST_DIR}/tests/tests.cpp
                ${CMAKE_CURRENT_LIST_DIR}/tests/tests-main.cpp
                ${CMAKE_CURRENT_LIST_DIR}/tests/Common/SchedulerKiller.cpp
        )

           target_compile_definitions(${PROJECT_NAME} PUBLIC UNIT_TESTS)

           target_include_directories(unittest_${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tests )

           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC COMPILE_HOST)

           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC LINUX_PORT_DEBUG)

           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC MODULE_CORE_CUSTOM_BUS)

           target_compile_options(unittest_${PROJECT_NAME} PUBLIC -fsanitize=address )



           target_link_libraries(unittest_${PROJECT_NAME} PRIVATE pthread ${PROJECT_NAME})

           target_link_options(unittest_${PROJECT_NAME} PUBLIC -fsanitize=address)


endif()

