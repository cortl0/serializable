/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_UNIT_TESTS_DEBUG_HELPER_HPP
#define SRLZ_UNIT_TESTS_DEBUG_HELPER_HPP

#include <iostream>

void debug_helper(char* buffer, size_t length)
{
    printf("buffer length: %llu\n", length);

    for (size_t i = 0; i < length; ++i)
        printf("buffer[%llu]\t%hhu\n", i, (uint8_t)*(buffer + i));
}

#endif // SRLZ_UNIT_TESTS_DEBUG_HELPER_HPP
