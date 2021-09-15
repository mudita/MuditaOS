add_library(utils STATIC)

target_include_directories(utils
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_sources(utils
	PRIVATE
		Utils.cpp
	PUBLIC
		Split.hpp
		Utils.hpp
)

target_link_libraries(utils
	PUBLIC
		log-api
		magic_enum
	PRIVATE
		hash-library::hash-library
)

