cmake_minimum_required(VERSION 3.16)

project(app.exe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()

file(GLOB_RECURSE SOURCES "src/*.cpp" "utils/*.cpp" "rays/*.cpp" "main.cpp")
qt_add_executable(app.exe ${SOURCES})

target_include_directories(app.exe PRIVATE src utils rays)
target_precompile_headers(app.exe PRIVATE "pch.hxx")
target_link_libraries(app.exe PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

set(UI_FILES main_window.ui)
qt_wrap_ui(QtUIExample ${UI_FILES})