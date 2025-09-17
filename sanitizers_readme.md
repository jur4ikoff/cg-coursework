
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
qt_standard_project_setup()

file(GLOB_RECURSE SOURCES "src/*.cpp" "utils/*.cpp" "rays/*.cpp" "qt_drawers/*.cpp" "main.cpp")
qt_add_executable(app.exe ${SOURCES})

target_include_directories(app.exe PRIVATE src utils rays qt_drawers)
target_precompile_headers(app.exe PRIVATE "pch.hxx")
target_link_libraries(app.exe PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

set(UI_FILES main_window.ui)
qt_wrap_ui(QtUIExample ${UI_FILES})
Как использовать:
1. Сборка с санитайзерами:
bash
# Создаем директорию для сборки
mkdir build
cd build

# Конфигурируем с адресным санитайзером
cmake .. -DENABLE_ASAN=ON

# Или с несколькими санитайзерами
cmake .. -DENABLE_ASAN=ON -DENABLE_UBSAN=ON

# Собираем проект
make -j4
2. Запуск с санитайзерами:
bash
# Просто запускаем приложение
./app.exe

# Для получения более детальной информации можно установить переменные окружения:
export ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1
export UBSAN_OPTIONS=print_stacktrace=1
./app.exe
3. Полезные переменные окружения для ASAN:
bash
# Включение обнаружения утечек
export ASAN_OPTIONS=detect_leaks=1

# Более строгая проверка инициализации
export ASAN_OPTIONS=check_initialization_order=1:strict_init_order=1

# Обнаружение использования стека после возврата
export ASAN_OPTIONS=detect_stack_use_after_return=1

# Вывод стека вызовов для UBSAN
export UBSAN_OPTIONS=print_stacktrace=1