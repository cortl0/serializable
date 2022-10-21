/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_MEMORY_H
#define SRLZ_MEMORY_H

#include <type_traits>

#include "base.hpp"

namespace srlz
{

/**
 * @brief you have to allocate and deallocate memory yourself
 */
class memory final : public base
{
public:
    virtual ~memory() = default;

    virtual bool serialize(
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {
        if (!write(static_cast<const void* const>(&size), sizeof(size_t), buffer, buffer_size, buffer_offset))
            return false;

        if (!write(static_cast<const void* const>(pointer), size, buffer, buffer_size, buffer_offset))
            return false;

        return true;
    }

    virtual bool deserialize(
        const char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {
        if (!read(static_cast<void* const>(const_cast<size_t* const>(&size)),
            sizeof(size_t), buffer, buffer_size, buffer_offset))
            return false;

        if (!read(static_cast<void* const>(pointer), size, buffer, buffer_size, buffer_offset))
            return false;

        return true;
    }

    unsigned char* pointer;
    size_t size;
};

} // namespace srlz

#endif // SRLZ_MEMORY_H
