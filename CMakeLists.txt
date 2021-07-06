cmake_minimum_required (VERSION 3.11)


project(minesweeper)

add_executable(minesweeper src/minesweeper.cpp src/board.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS} inc )

target_link_libraries(minesweeper ${GTKMM_LIBRARIES})

set_target_properties(minesweeper PROPERTIES CXX_STANDARD 17)
