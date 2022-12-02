project('utf8', 'cpp', version: '0.1.0', default_options : 'cpp_std=c++17')

lib_srcs = [
    'include/auc/codepoint.hpp',
    'include/auc/u8char.hpp',
    'src/u8char.cpp',
    'include/auc/u8text.hpp',
    'src/u8text.cpp',
]
lib = static_library(
    'aucutf8',
     include_directories: 'include',
     sources: lib_srcs
)

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')

test_srcs = [
    'test/main.cpp',
    'test/encoding_validity.cpp',
    'test/decoding_codepoint.cpp',
    'test/encoding_codepoint.cpp'
]
test = executable(
    'libaucutf8-test',
    include_directories: 'include',
    sources: test_srcs,
    dependencies: [
        gtest_dep
    ],
    link_with: [
        lib
    ]
)

test(
    'libaucutf8 unit tests',
    test
) 
