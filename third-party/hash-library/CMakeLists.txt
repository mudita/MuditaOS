
add_library(hash-library)
add_library(hash-library::hash-library ALIAS hash-library)
target_sources(hash-library
   PRIVATE
      src/crc32.cpp
      src/keccak.cpp
      src/md5.cpp
      src/sha1.cpp
      src/sha256.cpp
      src/sha3.cpp

   PUBLIC
      src/crc32.h
      src/hmac.h
      src/keccak.h
      src/md5.h
      src/sha1.h
      src/sha256.h
      src/sha3.h
    )

target_include_directories(hash-library
   PUBLIC
       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_options(hash-library PRIVATE "-Wno-error=unused-function")
