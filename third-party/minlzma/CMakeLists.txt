add_library(minlzma STATIC)

target_include_directories(minlzma
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_sources(minlzma
    PRIVATE
        src/minlzlib/inputbuf.c
        src/minlzlib/dictbuf.c
        src/minlzlib/lzma2dec.c
        src/minlzlib/lzmadec.c
        src/minlzlib/rangedec.c
        src/minlzlib/xzcrc.c
        src/minlzlib/xzstream.c
        src/minlzlib/lzmadec.h
        src/minlzlib/xzstream.h
        src/minlzlib/minlzlib.h
)

set_target_properties(minlzma PROPERTIES COMPILE_FLAGS "-Wno-multichar")
