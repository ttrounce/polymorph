cmake_minimum_required (VERSION 3.6)

add_subdirectory(glfw)

file(WRITE vendor.cpp "")
add_library(polymorph_vendor STATIC vendor.cpp)
target_link_libraries(polymorph_vendor glfw)

set_target_properties(polymorph_vendor PROPERTIES OUTPUT_NAME pmvend)
