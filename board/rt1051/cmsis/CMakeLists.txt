add_library(cmsis INTERFACE)

target_include_directories(cmsis
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_sources(cmsis
    INTERFACE
        arm_common_tables.h
        arm_const_structs.h
        arm_math.h
        CMakeLists.txt
        cmsis_armcc.h
        cmsis_armclang.h
        cmsis_compiler.h
        cmsis_gcc.h
        cmsis_iccarm.h
        cmsis_version.h
        core_armv8mbl.h
        core_armv8mml.h
        core_cm7.h
        MIMXRT1051_features.h
        MIMXRT1051.h
        MIMXRT1052_features.h
        MIMXRT1052.h
        mpu_armv7.h
        mpu_armv8.h
        system_MIMXRT1051.h
)
