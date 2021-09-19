project( utils-bootconfig VERSION 1.0
        DESCRIPTION "Library for processing boot config"
)

set ( SOURCES
    src/bootconfig.cpp
)

set( INCLUDES
    include
)

add_library( ${PROJECT_NAME} STATIC ${SOURCES} )

target_include_directories( ${PROJECT_NAME} PUBLIC ${INCLUDES} )

target_link_libraries( ${PROJECT_NAME}
   PRIVATE
      module-utils
      module-vfs
      platform
      hash-library::hash-library
      Microsoft.GSL::GSL
      json::json
)
