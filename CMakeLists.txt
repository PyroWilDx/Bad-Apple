cmake_minimum_required(VERSION 3.26)
project(BadApple)

set(CMAKE_CXX_STANDARD 17)

set(ROOT_DIR ".")

find_package(OpenCV REQUIRED)

include_directories(${ROOT_DIR}/include)

add_executable(BadApple ${ROOT_DIR}/src/Main.cpp
        ${ROOT_DIR}/src/Utils.cpp)

target_link_libraries(BadApple PRIVATE ${OpenCV_LIBS})
