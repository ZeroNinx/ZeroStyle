cmake_minimum_required(VERSION 3.20)
project(ZeroStylePackageConsumer LANGUAGES CXX)

find_package(ZeroStyle CONFIG REQUIRED)

add_executable(ZeroStyle_PackageConsumer
    main.cpp
)

target_link_libraries(ZeroStyle_PackageConsumer PRIVATE
    Zero::ZeroStyle
)
