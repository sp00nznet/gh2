# Guitar Hero II - ReXGlue Static Recompilation Project
cmake_minimum_required(VERSION 3.25)
project(gh2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find ReXGlue SDK
if(DEFINED ENV{REXSDK})
    set(REXSDK_PATH "$ENV{REXSDK}")
else()
    set(REXSDK_PATH "${CMAKE_SOURCE_DIR}/../tools/rexglue-sdk/out/install/win-amd64")
endif()
list(APPEND CMAKE_PREFIX_PATH "${REXSDK_PATH}")

find_package(rexglue REQUIRED)

# Include generated source list
if(EXISTS "${CMAKE_SOURCE_DIR}/../generated/sources.cmake")
    include(${CMAKE_SOURCE_DIR}/../generated/sources.cmake)
endif()

# Platform entry point from SDK
if(WIN32)
    set(ENTRY_POINT_SRC "${REXSDK_PATH}/share/rexglue/windowed_app_main_win.cpp")
else()
    set(ENTRY_POINT_SRC "${REXSDK_PATH}/share/rexglue/windowed_app_main_posix.cpp")
endif()

# Main executable
if(WIN32)
    add_executable(gh2 WIN32
        src/main.cpp
        src/stubs.cpp
        src/arkless.cpp
        src/settings.cpp
        src/menu.cpp
        src/keyboard_driver.cpp
        ${ENTRY_POINT_SRC}
        ${GENERATED_SOURCES}
    )
else()
    add_executable(gh2
        src/main.cpp
        src/stubs.cpp
        src/arkless.cpp
        src/settings.cpp
        src/menu.cpp
        src/keyboard_driver.cpp
        ${ENTRY_POINT_SRC}
        ${GENERATED_SOURCES}
    )
endif()

target_include_directories(gh2 PRIVATE
    ${CMAKE_SOURCE_DIR}/../generated
)

target_link_libraries(gh2 PRIVATE
    rex::core
    rex::runtime
    rex::kernel
    rex::graphics
    rex::ui
)

# Compile options matching rexglue requirements
target_compile_options(gh2 PRIVATE
    -fno-strict-aliasing
    -ffp-model=strict
    -fno-char8_t
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O3>
    $<$<CONFIG:RELEASE>:-DNDEBUG>
)

if(NOT MSVC)
    target_compile_options(gh2 PRIVATE -msse4.1)
endif()

# Allow our stubs.cpp to override kernel functions from rexkernel.lib
if(WIN32)
    target_link_options(gh2 PRIVATE "LINKER:/force:multiple")
endif()

# Console test executable (no GUI) for crash isolation
add_executable(gh2_test
    src/test_boot.cpp
    src/stubs.cpp
    src/arkless.cpp
    ${GENERATED_SOURCES}
)
target_include_directories(gh2_test PRIVATE
    ${CMAKE_SOURCE_DIR}/../generated
)
target_link_libraries(gh2_test PRIVATE
    rex::core
    rex::runtime
    rex::kernel
    rex::graphics
    rex::ui
)
target_compile_options(gh2_test PRIVATE
    -fno-strict-aliasing
    -ffp-model=strict
    -fno-char8_t
    $<$<CONFIG:RELEASE>:-O3>
    $<$<CONFIG:RELEASE>:-DNDEBUG>
)
if(NOT MSVC)
    target_compile_options(gh2_test PRIVATE -msse4.1)
endif()

if(WIN32)
    target_link_options(gh2_test PRIVATE "LINKER:/force:multiple")
endif()

# Whole-archive link for kernel hooks on Linux
if(NOT WIN32)
    target_link_options(gh2 PRIVATE
        -Wl,--whole-archive $<TARGET_FILE:rex::kernel> -Wl,--no-whole-archive
    )
    target_compile_options(gh2 PRIVATE -mcmodel=large)
endif()

# Codegen target - run 'cmake --build . --target gh2_codegen'
add_custom_target(gh2_codegen
    COMMAND $<TARGET_FILE:rex::rexglue> codegen ${CMAKE_SOURCE_DIR}/../config/gh2_rexglue.toml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
    COMMENT "Generating recompiled code for Guitar Hero II"
    VERBATIM
)
