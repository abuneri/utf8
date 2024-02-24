install(TARGETS auc 
    EXPORT auc-targets
    FILE_SET auc_public_headers
    ARCHIVE
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "auc-config-version.cmake"
    VERSION ${auc_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT auc-targets
    FILE auc-targets.cmake
    NAMESPACE auc::
    DESTINATION lib/cmake/auc
)

install(FILES
    "auc-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/auc-config-version.cmake"
    DESTINATION lib/cmake/auc
)