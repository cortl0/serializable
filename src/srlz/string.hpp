/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_STRING_HPP
#define SRLZ_STRING_HPP

#include <string>

#include "member.hpp"
#include "base.hpp"

namespace srlz
{

class string : public base, public std::string
{
public:
    virtual ~string() = default;

    void set(std::string value)
    {
        *((std::string*)this) = value;
    }

    virtual bool serialize(
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {
        const size_t length = this->length();

        if (!write(static_cast<const void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_offset))
            return false;

        if (!write(static_cast<const void* const>(c_str()), length, buffer, buffer_size, buffer_offset))
            return false;

        return true;
    }

    virtual bool deserialize(
        const char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {
        size_t length;
            
        if (!read(static_cast<void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_offset))
            return false;

        ((std::string*)this)->resize(length);

        if (!read(static_cast<void* const>(((std::string*)this)->data()), length, buffer, buffer_size, buffer_offset))
            return false;

        return true;
    }
};

} // namespace srlz

#endif // SRLZ_STRING_HPP
