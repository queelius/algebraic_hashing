# vcpkg portfile for algebraic-hashing
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO queelius/algebraic_hashing
    REF "v${VERSION}"
    SHA512 0 # This will be filled in by vcpkg maintainers
    HEAD_REF master
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        tests BUILD_TESTING
        examples BUILD_EXAMPLES
        benchmarks BUILD_BENCHMARKS
)

# Configure CMake
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        -DENABLE_CONCEPTS_CHECKING=ON
        -DENABLE_STATISTICS=ON
        -DCMAKE_CXX_STANDARD=20
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
    MAYBE_UNUSED_VARIABLES
        ENABLE_CONCEPTS_CHECKING
        ENABLE_STATISTICS
)

# Build (validation only for header-only library)
vcpkg_cmake_build()

# Install
vcpkg_cmake_install()

# Remove unnecessary CMake files from bin and debug/bin (if any)
vcpkg_cmake_config_fixup(
    PACKAGE_NAME AlgebraicHashing
    CONFIG_PATH lib/cmake/AlgebraicHashing
)

# Handle copyright and documentation
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")

# Copy additional documentation files
file(INSTALL "${SOURCE_PATH}/README.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${SOURCE_PATH}/CHANGELOG.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

# Header-only library - remove lib directories if they exist
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib" "${CURRENT_PACKAGES_DIR}/debug/lib")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# Ensure we don't have any binaries for header-only library
file(REMOVE_RECURSE 
    "${CURRENT_PACKAGES_DIR}/bin"
    "${CURRENT_PACKAGES_DIR}/debug/bin"
)