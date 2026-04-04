cmake_minimum_required(VERSION 3.20)
project(Rise)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# vcpkg Integration
set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed")
set(CMAKE_PREFIX_PATH "${VCPKG_ROOT}/x64-linux/share;${CMAKE_PREFIX_PATH}")
list(APPEND CMAKE_MODULE_PATH "${VCPKG_ROOT}/x64-linux/share/cmake")

# Include vcpkg-provided CMake modules
include(${VCPKG_ROOT}/x64-linux/share/vcpkg-cmake/vcpkg_cmake_configure.cmake OPTIONAL)
include(${VCPKG_ROOT}/x64-linux/share/vcpkg-cmake-config/vcpkg_cmake_config_fixup.cmake OPTIONAL)

find_package(spdlog CONFIG REQUIRED)

add_executable(Rise main.cpp)
target_link_libraries(Rise PRIVATE spdlog::spdlog)