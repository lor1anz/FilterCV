cmake_minimum_required(VERSION 3.20)

project(FilterCV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenCV REQUIRED)

set(SOURCES
  src/main.cpp
)

set(HEADERS
  include/globals.h
)

include_directories (${CMAKE_SOURCE_DIR}/include)

qt_add_executable(FilterCV ${SOURCES} ${HEADERS})

target_link_libraries(FilterCV PRIVATE Qt6::Widgets ${OpenCV_LIBS})
