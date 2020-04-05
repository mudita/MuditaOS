# add sources
set(PROTOBUF_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/src)
set(PROTOBUF ${PROTOBUF_SRCDIR}/google/protobuf)
set(PROTOBUF_SOURCES
        ${PROTOBUF}/any_lite.cc
        ${PROTOBUF}/arena.cc
        ${PROTOBUF}/extension_set.cc
        ${PROTOBUF}/generated_enum_util.cc
        ${PROTOBUF}/generated_message_table_driven_lite.cc
        ${PROTOBUF}/generated_message_util.cc
        ${PROTOBUF}/implicit_weak_message.cc
        ${PROTOBUF}/io/coded_stream.cc
        ${PROTOBUF}/io/io_win32.cc
        ${PROTOBUF}/io/strtod.cc
        ${PROTOBUF}/io/zero_copy_stream.cc
        ${PROTOBUF}/io/zero_copy_stream_impl.cc
        ${PROTOBUF}/io/zero_copy_stream_impl_lite.cc
        ${PROTOBUF}/message_lite.cc
        ${PROTOBUF}/parse_context.cc
        ${PROTOBUF}/repeated_field.cc
        ${PROTOBUF}/stubs/bytestream.cc
        ${PROTOBUF}/stubs/common.cc
        ${PROTOBUF}/stubs/int128.cc
        ${PROTOBUF}/stubs/status.cc
        ${PROTOBUF}/stubs/statusor.cc
        ${PROTOBUF}/stubs/stringpiece.cc
        ${PROTOBUF}/stubs/stringprintf.cc
        ${PROTOBUF}/stubs/structurally_valid.cc
        ${PROTOBUF}/stubs/strutil.cc
        ${PROTOBUF}/stubs/time.cc
        ${PROTOBUF}/wire_format_lite.cc
)
target_sources(${PROJECT_NAME} PRIVATE ${PROTOBUF_SOURCES})

# set compile definitions for third party libraries
target_compile_definitions(${PROJECT_NAME} PUBLIC GOOGLE_PROTOBUF_NO_THREADS)

# supress warning for protobuf
set_source_files_properties(${PROTOBUF_SOURCES}
        PROPERTIES COMPILE_FLAGS
	"-Wno-sign-compare -Wno-type-limits -Wno-redundant-move"
)

# add include dir
target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOBUF_SRCDIR})
