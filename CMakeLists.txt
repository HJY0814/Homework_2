cmake_minimum_required(VERSION 3.22)

project(Homework_2)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV_VERSION: ${OpenCV_VERSION}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")

add_executable(Homework_2 include/main.cpp
        include/Detector.cpp
        src/Detector.h)

target_include_directories(Homework_2 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Homework_2 PRIVATE ${OpenCV_LIBS} opencv_aruco)
