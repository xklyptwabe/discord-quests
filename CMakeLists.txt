cmake_minimum_required(VERSION 3.20)

project(
    DiscordQuests
    VERSION 1.0.0
    DESCRIPTION "Discord Custom Activity & Game Process Emulator"
    LANGUAGES CXX
)

# Настройка C++20 стандарта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Вывод бинарников в единую директорию
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Добавляем исходники
add_executable(DiscordQuests
    src/main.cpp
)

# Настройки компилятора под MSVC и Clang/GCC
target_compile_features(DiscordQuests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(DiscordQuests PRIVATE
        /W4          # Высокий уровень предупреждений
        /permissive- # Строгое соответствие стандарту C++
        /utf-8       # Поддержка UTF-8 кодировки
        /MP          # Многопоточная сборка
    )
else()
    target_compile_options(DiscordQuests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
