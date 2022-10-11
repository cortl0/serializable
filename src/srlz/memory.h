/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_MEMORY_H
#define SRLZ_MEMORY_H

#include <type_traits>

namespace srlz
{

/**
 * @brief you have to allocate and deallocate memory yourself
 */
struct memory
{
    unsigned char* pointer;
    size_t size;
};

} // namespace srlz

#endif // SRLZ_MEMORY_H
