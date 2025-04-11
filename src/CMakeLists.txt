add_library(calculator_lib
    calculator.cpp
)

target_include_directories(calculator_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(calculator
    main.cpp
)

target_link_libraries(calculator
    PRIVATE calculator_lib
)
