cmake_minimum_required(VERSION 4.4)
project(FSTM)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

find_package(OpenSSL REQUIRED)
target_link_libraries(app PRIVATE OpenSSL::Crypto)
