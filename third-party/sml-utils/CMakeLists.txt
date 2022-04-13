add_library(sml-logger INTERFACE) 
add_library(sml::utils::logger ALIAS sml-logger)

target_include_directories(sml-logger INTERFACE include/)
