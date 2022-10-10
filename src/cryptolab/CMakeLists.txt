cmake_minimum_required(VERSION 3.23)
project(libcryptolab)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. libcrypto_src)
add_library(libcryptolab ${libcrypto_src})
