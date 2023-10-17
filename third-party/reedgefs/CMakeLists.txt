
add_library(reedgefs INTERFACE)
add_library(reliance-edge::fs ALIAS reedgefs)

target_include_directories(reedgefs
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core/include>
)

target_sources( reedgefs
    INTERFACE
    src/bdev/bdev.c
    src/core/driver/blockio.c
    src/core/driver/buffer.c
    src/core/driver/buffercmn.c
    src/core/driver/core.c
    src/core/driver/dir.c
    src/core/driver/format.c
    src/core/driver/imap.c
    src/core/driver/imapextern.c
    src/core/driver/imapinline.c
    src/core/driver/inode.c
    src/core/driver/inodedata.c
    src/core/driver/volume.c
    src/fse/fse.c
    src/posix/path.c
    src/posix/posix.c
    src/util/bitmap.c
    src/util/crc.c
    src/util/endian.c
    src/util/memory.c
    src/util/namelen.c
    src/util/sign.c
    src/util/string.c

    src/include/redapimacs.h
    src/include/redbdev.h
    src/include/redconfigchk.h
    src/include/redcoreapi.h
    src/include/rederrno.h
    src/include/redexclude.h
    src/include/redformat.h
    src/include/redfs.h
    src/include/redfse.h
    src/include/redgetopt.h
    src/include/redmacs.h
    src/include/redmditer.h
    src/include/redmisc.h
    src/include/redosserv.h
    src/include/redpath.h
    src/include/redposix.h
    src/include/redstat.h
    src/include/redtests.h
    src/include/redtestutils.h
    src/include/redtoolcmn.h
    src/include/redtools.h
    src/include/redutils.h
    src/include/redver.h
    src/include/redvolume.h
)
