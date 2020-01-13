add_subdirectory(external/pdcurses)

add_library("${PROJECT_NAME}-lib" STATIC asm-curses.cpp)

target_link_libraries("${PROJECT_NAME}-lib" PUBLIC PDcurses)
target_include_directories("${PROJECT_NAME}-lib" PUBLIC "include")

install(TARGETS "${PROJECT_NAME}-lib" DESTINATION lib)
install(FILES "include/asm-curses.h" DESTINATION include)
