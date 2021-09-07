add_library(libphonenumber STATIC)
add_library(libphonenumber::libphonenumber ALIAS libphonenumber)

# choose metadata to use
option (LIBPHONENUMBER_USE_MINIMAL_METADATA "Use minimal metadata for libphonenumber" OFF)

target_sources(libphonenumber
    PRIVATE
        src/cpp/src/phonenumbers/asyoutypeformatter.cc
        src/cpp/src/phonenumbers/base/strings/string_piece.cc
        src/cpp/src/phonenumbers/default_logger.cc
        src/cpp/src/phonenumbers/logger.cc
        src/cpp/src/phonenumbers/phonemetadata.pb.cc
        src/cpp/src/phonenumbers/phonenumber.cc
        src/cpp/src/phonenumbers/phonenumber.pb.cc
        src/cpp/src/phonenumbers/phonenumberutil.cc
        src/cpp/src/phonenumbers/regex_based_matcher.cc
        src/cpp/src/phonenumbers/regexp_adapter_re2.cc
        src/cpp/src/phonenumbers/regexp_cache.cc
        src/cpp/src/phonenumbers/shortnumberinfo.cc
        src/cpp/src/phonenumbers/stringutil.cc
        src/cpp/src/phonenumbers/unicodestring.cc
        src/cpp/src/phonenumbers/utf/rune.c
        src/cpp/src/phonenumbers/utf/unicodetext.cc
        src/cpp/src/phonenumbers/utf/unilib.cc
        $<IF:$<BOOL:${LIBPHONENUMBER_USE_MINIMAL_METADATA}>,src/cpp/src/phonenumbers/metadata-minimal.cc,src/cpp/src/phonenumbers/metadata.cc>
)

target_include_directories(libphonenumber PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/src)

target_compile_definitions (libphonenumber
    PRIVATE
        I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
    PUBLIC
        I18N_PHONENUMBERS_USE_RTOS_WRAPPER
        I18N_PHONENUMBERS_USE_RE2
)

# suppress warning for libphonenumber
set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/src/phonenumbers/asyoutypeformatter.cc
    PROPERTIES COMPILE_FLAGS
        -Wno-implicit-fallthrough
)

target_link_libraries (libphonenumber 
    PRIVATE 
        re2::re2
    PUBLIC 
        protobuf::libprotobuf-lite
)
