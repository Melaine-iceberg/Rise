vcpkg_download_distfile(
    ARCHIVE
    URLS "https://www.bearssl.org/bearssl-0.6.tar.gz"
    FILENAME "bearssl-0.6.tar.gz"
    SHA512 f9ed25683cfc6c4abe7f1203a2b82ed101ee4c9e0f9ab60755b6a09c8d1e8e4f64d413624e7bb9c4b0033f909a2e4568a1d916cc6ce4736222900691e1f8359a
)

vcpkg_extract_source_archive(
    SOURCE_PATH
    ARCHIVE "${ARCHIVE}"
)

if(VCPKG_TARGET_IS_WINDOWS)
    message(FATAL_ERROR "The local bearssl overlay port currently supports Unix-like builds only.")
endif()

vcpkg_execute_build_process(
    COMMAND make lib STATICLIB=lib DLL=no TOOLS=no TESTS=no -j${VCPKG_CONCURRENCY}
    WORKING_DIRECTORY "${SOURCE_PATH}"
    LOGNAME "build-${TARGET_TRIPLET}"
)

file(INSTALL
    "${SOURCE_PATH}/build/libbearssl.a"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib"
)

file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/debug/lib")
file(INSTALL
    "${SOURCE_PATH}/build/libbearssl.a"
    DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib"
)

file(INSTALL
    DIRECTORY "${SOURCE_PATH}/inc/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING PATTERN "*.h"
)

file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(WRITE
    "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage"
    "The package bearssl installs headers and a static library.\n\n"
    "Typical CMake usage:\n\n"
    "    find_path(BEARSSL_INCLUDE_DIR bearssl.h REQUIRED)\n"
    "    find_library(BEARSSL_LIBRARY bearssl REQUIRED)\n"
    "    target_include_directories(main PRIVATE \${BEARSSL_INCLUDE_DIR})\n"
    "    target_link_libraries(main PRIVATE \${BEARSSL_LIBRARY})\n"
)

file(INSTALL
    "${SOURCE_PATH}/LICENSE.txt"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright
)
