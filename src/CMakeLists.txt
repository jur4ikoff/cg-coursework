cmake_minimum_required(VERSION 3.16)

project(app.exe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


# Опции для санитайзеров
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_COVERAGE "Enable code coverage for tests" OFF)

# Настройка санитайзеров
if(ENABLE_ASAN OR ENABLE_UBSAN OR ENABLE_TSAN)
    set(SANITIZER_FLAGS "")
    
    if(ENABLE_ASAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=address")
        # Для MacOS также нужна эта опция
        list(APPEND SANITIZER_FLAGS "-fno-omit-frame-pointer")
    endif()
    
    if(ENABLE_UBSAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=undefined")
        list(APPEND SANITIZER_FLAGS "-fno-sanitize-recover=all")
    endif()
    
    if(ENABLE_TSAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=thread")
    endif()
    
    # Добавляем флаги компиляции и линковки
    add_compile_options(${SANITIZER_FLAGS})
    add_link_options(${SANITIZER_FLAGS})
    
    # Для MacOS нужно добавить эти флаги для правильной работы
    if(APPLE)
        add_link_options(-fuse-ld=ld)
    endif()
    
    message(STATUS "Sanitizers enabled: ASAN=${ENABLE_ASAN}, UBSAN=${ENABLE_UBSAN}, TSAN=${ENABLE_TSAN}")
endif()


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(GTest REQUIRED)


 
qt_standard_project_setup()


file(GLOB_RECURSE SOURCES "window/*.cpp" "utils/*.cpp" "rays/*.cpp" "qt_drawer/*.cpp" "drawer/*.cpp" "dev/*.cpp" "render/*.cpp" "tasks/*.cpp" "models/*.cpp" "scene/*.cpp" "material/*.cpp" "threads/*.cpp")
file(GLOB_RECURSE APP_SOURCES  "main.cpp" ${SOURCES})
file(GLOB_RECURSE TEST_SOURCES  "tests/*.cpp" "utils/*.cpp")

list(FILTER TEST_SOURCES EXCLUDE REGEX "main\\.cpp$") # Удаляем main.cpp из тестовых источников

qt_add_executable(app.exe ${APP_SOURCES})
qt_add_executable(tests.exe ${TEST_SOURCES})



target_include_directories(app.exe PRIVATE window utils rays qt_drawer drawer models dev scene material threads tasks render)
target_precompile_headers(app.exe PRIVATE "pch.hxx")
target_link_libraries(app.exe PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Concurrent)

target_include_directories(tests.exe PRIVATE window utils rays qt_drawer drawer models dev scene material threads tasks render tests)
target_precompile_headers(tests.exe PRIVATE "pch.hxx")
target_link_libraries(tests.exe PRIVATE GTest::GTest GTest::Main Qt6::Core Qt6::Widgets Qt6::Gui)


if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(tests.exe PRIVATE --coverage)
    target_link_options(tests.exe PRIVATE --coverage)
    message(STATUS "Code coverage enabled for tests")
endif()


set(UI_FILES main_window.ui)
qt_wrap_ui(QtUIExample ${UI_FILES})
