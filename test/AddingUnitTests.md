Adding unit tests
=================

# Preface

We use either:
- [catch2](./Catch2/README.md)
- [google tests](./googletest/README.md)

to write unit tests.

You can add as many test targets as you wish.

**IMPORTANT:** All tests are run on the CI on PR's automatically

# How to use

Unit tests are always in catalog `test` or `tests` and should be included with 

**NOTE:** see the one letter difference!

```
if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
```

**NOTE:** To see how test gathering works please see: [CMake for UT](./CMakeLists.txt)

To add test target use either of functions:

`add_catch2_executable` or `add_gtest_executable`
 
Both take arguments:
- `NAME` test name, it will be prefixed with `catch2_` or `googletest_`, depending on your suite choice
- `SRCS` - sources for your unit tests
- `INCLUDE` - include directories (`-I`) for your test
- `LIBS` - libraries you depend on
- `DEFS` - defines you want to pass to the test
- `DEPS` - dependencies you want to pass to the test
- `USE_FS` if this unit test uses FS wrapper - **please** enable only if you really can't create the test without it, adding dependency to FS is not a good practice

# Simple example

**NOTE:** I assume here that:
- test catalog was created
- test catalog was included properly
If not - please read above!

Examplary executable can be added by creating `test/CMakeLists.txt` like that:
```
add_catch2_executable(
        NAME
            service-my_awesome_test
        SRCS
            test-MyAwesomeTest.cpp
        LIBS
)
```

Which will create target:
`catch2_service-my_awesome_test`

This means you can build it and run with:
`ninja catch2_service-my_awesome_test && catch2_service-my_awesome_test`
