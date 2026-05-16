cmake_minimum_required(VERSION 3.20)
project(ZeroStyle VERSION 1.0.0 LANGUAGES CXX)

# ==============================================================================
# 选项
# ==============================================================================

option(ZERO_USE_CXX23    "使用 C++23（可用时启用 std::expected 后端）" OFF)
option(ZERO_BUILD_TESTS  "编译冒烟测试"                          OFF)

# ==============================================================================
# C++ 标准
# ==============================================================================

if(ZERO_USE_CXX23)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Single-config generators (Ninja, Make): honour CMAKE_BUILD_TYPE.
# Multi-config generators (Visual Studio, Xcode): use --config at build time.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# MSVC: treat source files as UTF-8, suppress C4819
if(MSVC)
    add_compile_options(/utf-8)
endif()

# ==============================================================================
# Header-only INTERFACE 目标
# ==============================================================================

add_library(ZeroStyle INTERFACE)
add_library(Zero::ZeroStyle ALIAS ZeroStyle)

target_include_directories(ZeroStyle INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if(ZERO_USE_CXX23)
    target_compile_features(ZeroStyle INTERFACE cxx_std_23)
else()
    target_compile_features(ZeroStyle INTERFACE cxx_std_20)
endif()

# Clang 线程安全分析
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ZeroStyle INTERFACE -Wthread-safety)
endif()

# ==============================================================================
# 冒烟测试（-DZERO_BUILD_TESTS=ON）
# ==============================================================================

if(ZERO_BUILD_TESTS)
    enable_testing()
    add_executable(ZeroStyle_SmokeTest test/SmokeTest.cpp)
    target_link_libraries(ZeroStyle_SmokeTest PRIVATE Zero::ZeroStyle)
    add_test(NAME SmokeTest COMMAND ZeroStyle_SmokeTest)
endif()
