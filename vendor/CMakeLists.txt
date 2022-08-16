cmake_minimum_required (VERSION 3.6)

add_subdirectory(glfw)
add_subdirectory(VulkanMemoryAllocator)

find_package (Vulkan REQUIRED)

file(WRITE vendor.cpp "")
add_library(polymorph_vendor STATIC vendor.cpp)
target_link_libraries(polymorph_vendor glfw Vulkan::Vulkan opengl32 VulkanMemoryAllocator)

set_target_properties(polymorph_vendor PROPERTIES OUTPUT_NAME pmvend)
