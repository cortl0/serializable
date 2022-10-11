#
# brief project serializable
# author Ilya Shishkin (cortl@yandex.ru)
# license GPL v3.0
# copyright Copyright (c) 2022
#

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include(CTest)
enable_testing()

add_executable(TestSerializable serializable_test)

add_test(NAME TestSerializable
         COMMAND TestSerializable)
