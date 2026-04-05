cmake_minimum_required(VERSION 3.20)
project(Rise VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
pkg_check_modules(LIBURING REQUIRED IMPORTED_TARGET liburing)

file(GLOB_RECURSE CO_ASYNC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/co_async/*.cpp"
)

set(CO_ASYNC_SSL_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/co_async/iostream/ssl_socket_stream.cpp"
)
list(REMOVE_ITEM CO_ASYNC_SOURCES "${CO_ASYNC_SSL_SOURCE}")

find_path(BEARSSL_INCLUDE_DIR bearssl.h)
find_library(BEARSSL_LIBRARY bearssl)
set(CO_ASYNC_HAS_BEARSSL OFF)
if(BEARSSL_INCLUDE_DIR AND BEARSSL_LIBRARY)
    set(CO_ASYNC_HAS_BEARSSL ON)
    list(APPEND CO_ASYNC_SOURCES "${CO_ASYNC_SSL_SOURCE}")
endif()

add_library(co_async STATIC ${CO_ASYNC_SOURCES})
add_library(co_async::co_async ALIAS co_async)

target_include_directories(co_async
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(co_async
    PUBLIC
        PkgConfig::LIBURING
        ZLIB::ZLIB
)

target_compile_definitions(co_async
    PUBLIC
        CO_ASYNC_ZLIB=1
        $<$<BOOL:${CO_ASYNC_HAS_BEARSSL}>:CO_ASYNC_HAS_BEARSSL=1>
        $<$<NOT:$<BOOL:${CO_ASYNC_HAS_BEARSSL}>>:CO_ASYNC_HAS_BEARSSL=0>
)

if(CO_ASYNC_HAS_BEARSSL)
    target_include_directories(co_async PRIVATE "${BEARSSL_INCLUDE_DIR}")
    target_link_libraries(co_async PRIVATE "${BEARSSL_LIBRARY}")
else()
    message(STATUS "bearssl not found; building co_async without ssl_socket_stream.cpp")
endif()

set(CO_ASYNC_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/co_async")

install(
    TARGETS co_async
    EXPORT co_asyncTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/co_async"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
        PATTERN "*.hpp"
)

install(
    EXPORT co_asyncTargets
    FILE co_asyncTargets.cmake
    NAMESPACE co_async::
    DESTINATION "${CO_ASYNC_INSTALL_CMAKEDIR}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/co_asyncConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfig.cmake"
    INSTALL_DESTINATION "${CO_ASYNC_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfigVersion.cmake"
    DESTINATION "${CO_ASYNC_INSTALL_CMAKEDIR}"
)
