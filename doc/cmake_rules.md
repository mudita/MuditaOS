# How to use CMAKE

This document is based on:
Modern Cmake presentation from Deniz Bahadir:

[You Tube](https://www.youtube.com/watch?v=y9kSr5enrSk)

[Slides](https://raw.githubusercontent.com/Bagira80/More-Modern-CMake/master/OhNoMoreModernCMake.pdf)

[CMake Reference Documentation](https://cmake.org/cmake/help/latest/)

# Target Centric

Modern cmake approach is focused on "Tagets" and it properties.

## Crate a target

New targets should be created with:

- `add_executable`
- `add_library`

But we do not specify source files in this command, source files should be specified in `target_sources` commands.


## Build and Usage Requirements

Each target has two main requirements:

- Build requirement - **needed when building** (usually targets/files/includes)
- Usage requirement - **needef when using** (ex.: public headers)

We can specify requirements with following commands:

```cmake
target_include_directories( <target> PRIVATE|INTERFACE|PUBLIC <include-search-dir>... )
target_compile_definitions( <target> PRIVATE|INTERFACE|PUBLIC <macro-definitions>... )
target_compile_options(     <target> PRIVATE|INTERFACE|PUBLIC <compiler-option>... )
target_compile_features(    <target> PRIVATE|INTERFACE|PUBLIC <feature>... )
target_sources(             <target> PRIVATE|INTERFACE|PUBLIC <source-file>... )
target_precompile_headers(  <target> PRIVATE|INTERFACE|PUBLIC <header-file>... )
target_link_libraries(      <target> PRIVATE|INTERFACE|PUBLIC <dependency>... )
target_link_options(        <target> PRIVATE|INTERFACE|PUBLIC <linker-option>... )
target_link_directories(    <target> PRIVATE|INTERFACE|PUBLIC <linker-search-dir>... )
```

Depending on which scope keyword we use (PRIVATE, INTERFACE or PUBLIC) we define different usage:

- `PRIVATE`   - build requirement: fallowing items are needed for target to be build
- `INTERFACE` - usage requirement: fallowing items are needed for using the target (ex.: public headers)
- `PUBLIC`    - both build an usage requirements.

Warning: `target_link_libraries` can be used without these keywords, pleas don't!

## Adding source files

For both `add_executable` and `add_library` one can specify source files in the command,
please don't, this is old behaviour that should be avoided.

### PRIVATE, INTERFACE or PUBLICV
For adding sources use `target_source` command with proper usage keywords (PRIVATE|INTERFACE|PUBLIC). Add both source and header files, each to proper section:

PRIVATE - usually all `*.cpp` - we need them to build the object, 
and all header files that are used only by the library it self.

INTERFACE - files that are needed to use the library, ex.: public headers

PUBLIC - files that we need both for compilation and later for using ex.: class headers, templates, etc.

Why?
- CMake will maintain all `-I` `-L` options when you use your target later in `target_link_libraries`
- Most IDEs that understand CMake will be able to properly display files

### ${CMAKE_CURRENT_SOURCE_DIR}

`${CMAKE_CURRENT_SOURCE_DIR}` is default search path, 
You don't have to ad it in the path.

### GLOB
Using `file(GLOB ...` **is forbidden**! TL;DR: glob requires 
manual reconfiguration of the project each time you add a file.

Long answer, glob is run during cmake configuration. 
When you add a new source file (or resource) and you 
are using globing, nothing happens -> because you 
haven't change `CMakeLists.txt` file, there's no 
reason to refresh this file.

You would have to rerun configuration manually form
scratch and even this may not work because of 
caching, and you would need to delete build dir to fix it.

If you explicitly write your new file to CMakeList.txt 
(proper `target_source` section), you change the file, 
CMake can detect the change and reconfigure it 
(usually only dependent files and not entire project).

# Object fils

`add_library( obj OBJECT)` is still special beast in CMake, let's not use it.

# Coding style

- function names should be written using small letter
- variables preferred capital letters
- use spaces not tabs
