cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

find_package(OpenCV REQUIRED)

MESSAGE("OpenCV ==: "${OpenCV})
MESSAGE("REQUIRED ==: "${REQUIRED})
MESSAGE("OpenCV_LIBRARIES ==: "${OpenCV_LIBRARIES})

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)

add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp Triangle.hpp Triangle.cpp)
target_link_libraries(Rasterizer ${OpenCV_LIBRARIES})
