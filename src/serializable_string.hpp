/**
 * @file serializable_string.hpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable
 * @version 0.1
 * @date 2022-09-13
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_SERIALIZABLE_STRING_HPP
#define SRLZ_SERIALIZABLE_STRING_HPP

#include <string>

#include "member.hpp"
#include "serializable_base.hpp"

namespace srlz
{

class serializable_string : public serializable_base, public std::string
{
public:
    //virtual ~serializable_string() = default;

    void set_member(void* member)
    {
        member_ = member;
    }

    virtual bool serialize(
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_shift
        ) const override
    {
        auto& mem = *static_cast<member<std::string, member_type::STD_STRING>*>(member_);

        const size_t length = mem.value_.length();

        if (!write(static_cast<const void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_shift))
            return false;

        if (!write(static_cast<const void* const>(mem.value_.c_str()), length, buffer, buffer_size, buffer_shift))
            return false;

        return true;
    }

    virtual bool deserialize(
        const char* const buffer,
        const size_t buffer_size,
        size_t& buffer_shift
        ) const override
    {
        auto& mem = *static_cast<member<std::string, member_type::STD_STRING>*>(member_);

        size_t length;
            
        if (!read(static_cast<void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_shift))
            return false;

        mem.value_.resize(length);

        if (!read(static_cast<void* const>(mem.value_.data()), length, buffer, buffer_size, buffer_shift))
            return false;

        return true;
    }

private:
    void* member_;
};

} // namespace srlz

#endif // SRLZ_SERIALIZABLE_STRING_HPP
