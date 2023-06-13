SET(ENABLE_GTK_DOC OFF)
SET(STATIC_ONLY TRUE)
SET(ICAL_BUILD_DOCS FALSE)
SET(ICAL_GLIB FALSE)
SET(LIBICAL_BUILD_TESTING FALSE)

include(libical/cmake/modules/LibIcalMacrosInternal.cmake)

# Turn off false positive errors caused by calls to strncpy and snprintf.
libical_add_cflag(-Wno-format-truncation NO_FORMAT_TRUNCATION)
libical_add_cflag(-Wno-stringop-truncation NO_STRINGOP_TRUNCATION)

# Turn off false positive errors for debug build.
libical_add_cflag(-Wno-maybe-uninitialized NO_MAYBE_UNINITIALIZED)

add_subdirectory(libical)
