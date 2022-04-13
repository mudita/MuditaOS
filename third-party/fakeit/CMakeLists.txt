add_library(fakeit INTERFACE) 
add_library(test::fakeit ALIAS fakeit)

target_include_directories(fakeit INTERFACE FakeIt/single_header/) 
