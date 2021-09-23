add_library(sys-common INTERFACE)

target_include_directories(sys-common
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_sources(sys-common
    INTERFACE
        include/system/Common.hpp
        include/system/Constants.hpp
        include/system/messages/TetheringQuestionRequest.hpp
        include/system/messages/RequestCpuFrequencyMessage.hpp
        include/system/messages/SentinelRegistrationMessage.hpp
        include/system/messages/DeviceRegistrationMessage.hpp
        include/system/messages/TetheringStateRequest.hpp
        include/system/messages/SystemManagerMessage.hpp
        include/system/SystemReturnCodes.hpp
)
