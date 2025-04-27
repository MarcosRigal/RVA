cmake_minimum_required (VERSION 3.0)
project (ARBlockT1)
set(CMAKE_CXX_FLAGS "-std=c++11")
find_package(OpenCV 4.5 REQUIRED)

add_executable(kps kps.cpp rva.cpp)
target_link_libraries(kps ${OpenCV_LIBS})
