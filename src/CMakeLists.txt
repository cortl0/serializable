#
# file serializable.hpp
# author Ilya Shishkin (cortl@yandex.ru)
# brief project serializable
# version 0.1
# date 2022-09-11
# license GPL v3.0
# copyright Copyright (c) 2022
#

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_executable(TestSerializable serializable_test.cpp)
add_test(NAME TestSerializable
         COMMAND TestSerializable)
