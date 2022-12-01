project('utf8', 'cpp', version: '0.1.0', default_options : 'cpp_std=c++17')

srcs = ['src/u8char.cpp', 'src/u8text.cpp']
lib = static_library(
    'libaucutf8',
     include_directories: 'include',
     sources: srcs
)

utest = executable(
    'utf8-test',
    include_directories: 'include',
    sources:'test/main.cpp',
    link_with: [
        lib
    ]
)

test(
    'utf8 unit tests',
    utest
) 
