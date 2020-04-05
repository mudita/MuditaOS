# add sources
set (LIBPHONENUMBER_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/libphonenumber/cpp/src)
set (LIBPHONENUMBER ${LIBPHONENUMBER_SRCDIR}/phonenumbers)
set (LIBPHONENUMBER_SOURCES
        ${LIBPHONENUMBER}/asyoutypeformatter.cc
        ${LIBPHONENUMBER}/base/strings/string_piece.cc
        ${LIBPHONENUMBER}/default_logger.cc
        ${LIBPHONENUMBER}/logger.cc
        ${LIBPHONENUMBER}/metadata.cc
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
)
target_sources(${PROJECT_NAME} PRIVATE ${LIBPHONENUMBER_SOURCES})

# set compile definitions
target_compile_definitions(${PROJECT_NAME}
        PRIVATE
                I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
        PUBLIC
                I18N_PHONENUMBERS_NO_THREAD_SAFETY
                I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
                I18N_PHONENUMBERS_USE_RE2
)

# suppress warning for libphonenumber
set_source_files_properties(${LIBPHONENUMBER}/asyoutypeformatter.cc
        PROPERTIES COMPILE_FLAGS
        -Wno-implicit-fallthrough
)

# add include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBPHONENUMBER_SRCDIR})
