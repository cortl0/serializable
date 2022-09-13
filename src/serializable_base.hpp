/**
 * @file serializable_base.hpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable
 * @version 0.1
 * @date 2022-09-13
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_SERIALIZABLE_BASE_HPP
#define SRLZ_SERIALIZABLE_BASE_HPP

#include <type_traits>

namespace srlz
{

class serializable_base
{
public:
    virtual ~serializable_base() = default;

    virtual bool serialize(
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_shift
        ) const = 0;

    virtual bool deserialize(
        const char* const buffer,
        const size_t buffer_size,
        size_t& buffer_shift
        ) const = 0;

protected:
    bool write(
        const void* const value,
        const size_t value_length,
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_shift
        ) const
    {
        if (buffer_shift + value_length > buffer_size)
            return false;

        std::memcpy(buffer + buffer_shift, value, value_length);
        buffer_shift += value_length;

        return true;
    };

    bool read(
        void* const value,
        const size_t value_length,
        const char* const buffer,
        const size_t buffer_size,
        size_t& buffer_shift
        ) const
    {
        if (buffer_shift + value_length > buffer_size)
            return false;

        std::memcpy(value, buffer + buffer_shift, value_length);
        buffer_shift += value_length;

        return true;
    };
};

} // namespace srlz

#endif // SRLZ_SERIALIZABLE_BASE_HPP
