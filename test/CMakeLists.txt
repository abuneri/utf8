enable_testing()

#===============================================
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # 1.14.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

#===============================================
add_executable(auc_test
    main.cpp
    
    graphemebreaktest_data.hpp
    
    encoding_validity.cpp
    decoding_codepoint.cpp
    encoding_codepoint.cpp
    parse_bom.cpp
    grapheme_clusters.cpp
)
target_link_libraries(auc_test
    auc
    GTest::GTest
)
add_test(auc_tests
    auc_test
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
