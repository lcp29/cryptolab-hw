cmake_minimum_required(VERSION 3.23)
project(cryptolab)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src src_dirs)
aux_source_directory(src/cryptolab cryptolab_dirs)
add_executable(cryptolab ${src_dirs} ${cryptolab_dirs} src/cryptolab/ByteBuffer.cpp)
