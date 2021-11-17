project(hello)
cmake_minimum_required(VERSION 3.2)

include_directories(/opt/include)

add_library(hello MODULE hello.cpp)
target_compile_features(hello PRIVATE cxx_range_for cxx_auto_type)
set_target_properties(hello PROPERTIES COMPILE_FLAGS "-fno-rtti")