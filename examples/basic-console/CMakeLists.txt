add_executable(ZeroStyle_BasicConsole
    main.cpp
)

target_include_directories(ZeroStyle_BasicConsole PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ZeroStyle_BasicConsole PRIVATE
    Zero::ZeroStyle
)

zero_apply_project_warnings(ZeroStyle_BasicConsole)
