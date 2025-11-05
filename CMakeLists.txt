cmake_minimum_required(VERSION 3.15)
project(CryptoMath VERSION 1.0.0 LANGUAGES CXX)

# Устанавливаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем все предупреждения
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Добавляем include директорию
include_directories(${CMAKE_SOURCE_DIR}/include)

# Примеры
add_executable(test_basic examples/test_basic.cpp)
target_include_directories(test_basic PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Устанавливаем выходные директории
set_target_properties(test_basic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)