Third-party libraries
=====================

Third-party libraries which can be included in MuditaOS builds for each and every target

# How to add 3rd-party target

Please mind that we are using CMake library encapsulation technique where we:
- create interface for the library in cmake
- build the library with the interface
- export only the required interface

# Examples

## header only library

- we use `awesome-library` for library name in this example:
- create folder for library
- add CMakeLists.txt for library with, where header only lib is in `include` catalog i.e. under: `include/awseome-library/awesome-library.hpp`
```
add_library(awesome-library INTERFACE) 
add_library(awesome::library::alias ALIAS awesome-library)

target_include_directories(awesome-library INTERFACE include/)
```

**NOTE:** You can check out how [sml-utils cmake](sml-utils/CMakeLists.txt) and catalogs are created

## normal libraries

Please note:
- name `awesome-library` in use here.
- we use `PRIVATE` in cmake sections while adding sources

How to create the target depends on library, exemplary cmake example can look like that:

```
add_library(awesome-library)
target_sources(awesome-library
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/awesome-library.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/awesome-library.h
)

target_include_directories(awesome-library
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

add_library(awesome::library ALIAS awesome-library)
```

Where in `src` there is `awesome-library/awesome-library.hpp` header
