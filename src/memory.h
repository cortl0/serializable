/**
 * @file serializable.hpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable
 * @version 0.1
 * @date 2022-09-11
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_MEMORY_HPP
#define SRLZ_MEMORY_HPP

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

#endif // SRLZ_MEMORY_HPP
