cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME "starflut")
project(${PROJECT_NAME} LANGUAGES CXX C)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "starflut_plugin")

add_library(${PLUGIN_NAME} SHARED
  "starflut_plugin.cpp"
  "starflut_plugin_common.cpp"  
  "md5proc.cpp"
  "srplib_main.cpp"
  "vs_shell.c"
)
apply_standard_settings(${PLUGIN_NAME})
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/starcore")  
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin)

#add by starcore
set(CMAKE_CXX_FLAGS "-DENV_WINDOWS -D_CRT_SECURE_NO_WARNINGS ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-DENV_WINDOWS -DENV_M64 -D_CRT_SECURE_NO_WARNINGS ${CMAKE_C_FLAGS}")

# List of absolute paths to libraries that should be bundled with the plugin
set(starflut_bundled_libraries
  ""
  "${CMAKE_CURRENT_SOURCE_DIR}/starcore"
  PARENT_SCOPE
)

