include(Assets)
include(Utils)

set(SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(ASSETS_DEST_DIR ${SYSROOT}/sys)


#multicomp_install(
#    FILES
#        ${ASSETS_DEST_DIR}/.boot.json
#        ${ASSETS_DEST_DIR}/.boot.json.crc32
#    DESTINATION ./
#    COMPONENTS Standalone Update
#)

#multicomp_install(
#    DIRECTORY
#        ${ASSETS_DEST_DIR}/current/
#        ${ASSETS_DEST_DIR}/user
#    DESTINATION ./
#    COMPONENTS Standalone Update
#)
