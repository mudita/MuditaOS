﻿include(Assets)
include(Utils)

set(SYSTEM_DEST_DIR ${SYSROOT_PATH}/system_a)
set(USER_DEST_DIR ${SYSROOT_PATH}/user)

set(ASSETS_DEPENDENCIES "json-common-target")
list(APPEND ASSETS_DEPENDENCIES "create_product_databases")

if (${ASSETS_TYPE} STREQUAL "Proprietary")
    list(APPEND ASSETS_DEPENDENCIES "json-proprietary-target")
endif()

if (${ASSETS_TYPE} STREQUAL "Community")
    list(APPEND ASSETS_DEPENDENCIES "json-community-target")
endif()

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    list(APPEND ASSETS_DEPENDENCIES "json-rt1051-target")
endif()

add_assets_target(
    TARGET assets
    SOURCE_DIR ${ASSETS_SOURCE_DIR}
    SYSTEM_DEST_DIR ${SYSTEM_DEST_DIR}
    USER_DEST_DIR ${USER_DEST_DIR}
    DEVEL ${WITH_DEVELOPMENT_FEATURES}
    DEPENDS ${ASSETS_DEPENDENCIES}
)
