add_subdirectory(external/pdcurses)

add_library("${PROJECT_NAME}-lib" STATIC ncurses_controller.cpp)

target_link_libraries("${PROJECT_NAME}-lib" PUBLIC PDcurses)
target_include_directories("${PROJECT_NAME}-lib" PUBLIC "include")

install(TARGETS "${PROJECT_NAME}-lib" DESTINATION lib)
install(FILES "include/ncurses_controller.h" DESTINATION include)
