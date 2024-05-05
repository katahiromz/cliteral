# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.6)

# project name and languages
project(cliteral CXX RC)

##############################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_C_FLAGS "-static -O3 -Wall -pedantic")
        set(CMAKE_CXX_FLAGS "-static -O3 -Wall -pedantic")
    else()
        set(CMAKE_C_FLAGS "-static -g -Wall -pedantic")
        set(CMAKE_CXX_FLAGS "-static -g -Wall -pedantic")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_C_FLAGS "-s -static -O3 -Wall -pedantic")
        set(CMAKE_CXX_FLAGS "-s -static -O3 -Wall -pedantic")
    else()
        set(CMAKE_C_FLAGS "-static -g -Wall -pedantic")
        set(CMAKE_CXX_FLAGS "-static -g -Wall -pedantic")
    endif()
elseif (MSVC AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    # replace "/MD" with "/MT" (building without runtime DLLs)
    set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    foreach(CompilerFlags ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
    endforeach()
endif()

##############################################################################
# Unicode

add_definitions(-DUNICODE -D_UNICODE)

##############################################################################

# libutfio.a
add_library(utfio STATIC utfio.cpp)

# utfio_tests.exe
add_executable(utfio_tests utfio_tests.cpp)
target_link_libraries(utfio_tests PRIVATE utfio)

# libcstr.a
add_library(cstr STATIC CSTR_dump.cpp CSTR_parse.cpp)

# CSTR_dump_tests.exe
add_executable(CSTR_dump_tests CSTR_dump_tests.cpp)
target_link_libraries(CSTR_dump_tests PRIVATE cstr utfio)

# CSTR_parse_tests.exe
add_executable(CSTR_parse_tests CSTR_parse_tests.cpp)
target_link_libraries(CSTR_parse_tests PRIVATE cstr utfio)

# cliteral.exe
add_executable(cliteral WIN32 cliteral.cpp cliteral_res.rc)
target_link_libraries(cliteral PRIVATE cstr utfio shlwapi comctl32)
if(MSVC)
    target_link_options(cliteral PRIVATE /MANIFEST:NO)
endif()

##############################################################################
