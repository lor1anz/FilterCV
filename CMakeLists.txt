cmake_minimum_required(VERSION 3.20)

project(FilterCV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)

set(SOURCES
  src/main.cpp
  
  # gui
  src/gui/main_window.cpp
  src/gui/image_widget.cpp

  # core
  src/core/cv_engine.cpp

  # system
  src/system/screen.cpp
)

set(HEADERS
  include/globals.h
  
  # gui
  include/gui/main_window.h
  include/gui/image_widget.h
  include/gui/utils.h

  # filters
  include/filters/filter.h
  include/filters/grayscale.h
  include/filters/blur.h
  include/filters/canny.h
  include/filters/jpeg.h
  include/filters/sharpen.h
  include/filters/pixel_sort.h

  include/filters/glitch.h

  # core
  include/core/cv_engine.h

  # system
  include/system/screen.h
)

include_directories (${CMAKE_SOURCE_DIR}/include)

qt_add_executable(FilterCV ${SOURCES} ${HEADERS})

target_link_libraries(FilterCV PRIVATE X11 Qt6::Widgets ${OpenCV_LIBS} X11::X11 X11::Xrandr)
