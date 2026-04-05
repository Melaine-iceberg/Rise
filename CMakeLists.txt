cmake_minimum_required(VERSION 3.20)

project(co_async VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
pkg_check_modules(LIBURING REQUIRED IMPORTED_TARGET liburing)

set(CO_ASYNC_SOURCES
	co_async/generic/allocator.cpp
	co_async/generic/generic_io.cpp
	co_async/generic/io_context.cpp
	co_async/generic/io_context_mt.cpp
	co_async/generic/thread_pool.cpp
	co_async/iostream/directory_stream.cpp
	co_async/iostream/file_stream.cpp
	co_async/iostream/pipe_stream.cpp
	co_async/iostream/ssl_socket_stream.cpp
	co_async/iostream/stdio_stream.cpp
	co_async/iostream/zlib_stream.cpp
	co_async/net/http_protocol.cpp
	co_async/net/http_server.cpp
	co_async/net/http_server_utils.cpp
	co_async/net/http_string_utils.cpp
	co_async/net/socket_proxy.cpp
	co_async/net/uri.cpp
	co_async/platform/platform_io.cpp
	co_async/platform/socket.cpp
)

add_library(co_async ${CO_ASYNC_SOURCES})
add_library(co_async::co_async ALIAS co_async)

target_compile_features(co_async PUBLIC cxx_std_20)

target_include_directories(co_async
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(co_async
	PUBLIC
		CO_ASYNC_DEBUG=0
		CO_ASYNC_DIRECT=0
		CO_ASYNC_INVALFIX=0
		CO_ASYNC_ZLIB=1
)

target_link_libraries(co_async
	PUBLIC
		ZLIB::ZLIB
		PkgConfig::LIBURING
		bearssl
)

set_target_properties(co_async PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(
	TARGETS co_async
	EXPORT co_asyncTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/co_async
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING
		PATTERN "*.hpp"
)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/co_asyncConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/co_async
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(
	EXPORT co_asyncTargets
	FILE co_asyncTargets.cmake
	NAMESPACE co_async::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/co_async
)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/co_asyncConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/co_async
)
