cmake_minimum_required(VERSION "3.16.2")

project(asm-curses)

add_subdirectory(src)

add_executable("${PROJECT_NAME}-example" "examples/main.cpp")

target_link_libraries("${PROJECT_NAME}-example" "${PROJECT_NAME}-lib" )

install(TARGETS "${PROJECT_NAME}-example" DESTINATION bin) 

include(CTest)

add_test("Test1" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-example")
