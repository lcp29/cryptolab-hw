cmake_minimum_required(VERSION 3.23)
project(cryptolab-lab1)

set(CMAKE_CXX_STANDARD 17)

include_directories(cryptolab-lab1 ../cryptolab)

add_executable(cryptolab-lab1 lab1.cpp)
target_link_libraries(${PROJECT_NAME} libcryptolab)
