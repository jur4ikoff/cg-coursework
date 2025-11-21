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