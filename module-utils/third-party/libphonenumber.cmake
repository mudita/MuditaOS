include (thirdparty)

# choose metadata to use
option (LIBPHONENUMBER_USE_MINIMAL_METADATA "Use minimal metadata for libphonenumber" OFF)

# add sources
set (LIBPHONENUMBER_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/libphonenumber/cpp/src)
set (LIBPHONENUMBER ${LIBPHONENUMBER_SRCDIR}/phonenumbers)
if (LIBPHONENUMBER_USE_MINIMAL_METADATA)
        set (LIBPHONENUMBER_METADATA ${LIBPHONENUMBER}/metadata-minimal.cc)
else ()
        set (LIBPHONENUMBER_METADATA ${LIBPHONENUMBER}/metadata.cc)
endif ()
set (LIBPHONENUMBER_SOURCES
        ${LIBPHONENUMBER}/asyoutypeformatter.cc
        ${LIBPHONENUMBER}/base/strings/string_piece.cc
        ${LIBPHONENUMBER}/default_logger.cc
        ${LIBPHONENUMBER}/logger.cc
        ${LIBPHONENUMBER}/phonemetadata.pb.cc
        ${LIBPHONENUMBER}/phonenumber.cc
        ${LIBPHONENUMBER}/phonenumber.pb.cc
        ${LIBPHONENUMBER}/phonenumberutil.cc
        ${LIBPHONENUMBER}/regex_based_matcher.cc
        ${LIBPHONENUMBER}/regexp_adapter_re2.cc
        ${LIBPHONENUMBER}/regexp_cache.cc
        ${LIBPHONENUMBER}/shortnumberinfo.cc
        ${LIBPHONENUMBER}/stringutil.cc
        ${LIBPHONENUMBER}/unicodestring.cc
        ${LIBPHONENUMBER}/utf/rune.c
        ${LIBPHONENUMBER}/utf/unicodetext.cc
        ${LIBPHONENUMBER}/utf/unilib.cc
        ${LIBPHONENUMBER_METADATA}
)

# create static library for the third party
set (LIBPHONENUMBER_TARGET phonenumber)
add_library (${LIBPHONENUMBER_TARGET} STATIC ${LIBPHONENUMBER_SOURCES})

# setup flags for the third party
third_party_target_setup (${LIBPHONENUMBER_TARGET})

# set compile definitions
target_compile_definitions (${LIBPHONENUMBER_TARGET}
        PRIVATE
                I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
        PUBLIC
                I18N_PHONENUMBERS_USE_RTOS_WRAPPER
                I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
                I18N_PHONENUMBERS_USE_RE2
)

# suppress warning for libphonenumber
set_source_files_properties (${LIBPHONENUMBER}/asyoutypeformatter.cc
        PROPERTIES COMPILE_FLAGS
        -Wno-implicit-fallthrough
)

# add include directory path
target_include_directories (${LIBPHONENUMBER_TARGET} PUBLIC ${LIBPHONENUMBER_SRCDIR})

# module-os dependency (locking support)
target_link_libraries (${LIBPHONENUMBER_TARGET} PUBLIC module-os)

# RE2 dependency
if (NOT RE2_TARGET)
        message (FATAL_ERROR "RE2 is required for libphonenumber")
endif ()
target_link_libraries (${LIBPHONENUMBER_TARGET} PUBLIC ${RE2_TARGET})

target_link_libraries (${LIBPHONENUMBER_TARGET} PUBLIC protobuf::libprotobuf-lite)

# turn on optimization in debug
third_party_source_optimization (${LIBPHONENUMBER_SOURCES})
