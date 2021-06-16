# choose metadata to use
option (LIBPHONENUMBER_USE_MINIMAL_METADATA "Use minimal metadata for libphonenumber" OFF)

if (LIBPHONENUMBER_USE_MINIMAL_METADATA)
        set (LIBPHONENUMBER_METADATA src/cpp/src/phonenumbers/metadata-minimal.cc)
else ()
        set (LIBPHONENUMBER_METADATA src/cpp/src/phonenumbers/metadata.cc)
endif ()

# using variable to setup sources to be later used for optimalisation enabling macro
set (LIBPHONENUMBER_SOURCES
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
        ${LIBPHONENUMBER_METADATA}
)

# create static library for the third party
add_library (phonenumber STATIC ${LIBPHONENUMBER_SOURCES})

# setup flags for the third party
third_party_target_setup (phonenumber)

# set compile definitions
target_compile_definitions (phonenumber
        PRIVATE
                I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
        PUBLIC
                I18N_PHONENUMBERS_USE_RTOS_WRAPPER
                I18N_PHONENUMBERS_ASCII_DECIMALS_ONLY
                I18N_PHONENUMBERS_USE_RE2
)

# suppress warning for libphonenumber
set_source_files_properties (src/cpp/src/phonenumbers/asyoutypeformatter.cc
        PROPERTIES COMPILE_FLAGS
        -Wno-implicit-fallthrough
)

# add include directory path
target_include_directories (phonenumber PUBLIC src/cpp/src)

target_link_libraries (phonenumber 
        PRIVATE 
                re2::re2
        PUBLIC 
                protobuf::libprotobuf-lite
)

# turn on optimization in debug
third_party_source_optimization (${LIBPHONENUMBER_SOURCES})
