add_library( service-fileindexer STATIC )

target_sources( service-fileindexer
	PRIVATE
        Common.hpp
        InotifyHandler.cpp
        ServiceFileIndexer.cpp
        StartupIndexer.cpp
    PUBLIC
        include/service-fileindexer/Constants.hpp
        include/service-fileindexer/InotifyHandler.hpp
        include/service-fileindexer/ServiceFileIndexer.hpp
        include/service-fileindexer/StartupIndexer.hpp
)

target_include_directories( service-fileindexer
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/
)

target_link_libraries(service-fileindexer 
	PRIVATE
		utf8
        tagsfetcher
        module-bsp 
		module-os 
		module-utils 
		module-vfs 
		module-sys 
		tag
)
