/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_VECTOR_HPP
#define SRLZ_VECTOR_HPP

#include <memory>
#include <vector>

#include "member_type.h"
#include "base.hpp"

namespace srlz
{

template<typename _Tp/*, typename _Alloc = std::allocator<_Tp> */>
class vector final : public base, public std::vector<std::unique_ptr<_Tp>/*, _Alloc*/>
{
public:
    virtual ~vector() = default;

    virtual bool serialize(
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {
        const size_t length = this->size();

        if (!write(static_cast<const void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_offset))
            return false;

        for (auto& item : *((std::vector<std::unique_ptr<_Tp>>*)this))
            if(!item->serialize(buffer, buffer_size, buffer_offset))
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

        ((std::vector<std::unique_ptr<_Tp>>*)this)->clear();

        for (; length > 0; --length)
        {
            _Tp* item = new _Tp();

            if (!item->deserialize(buffer, buffer_size, buffer_offset))
                return false;

            ((std::vector<std::unique_ptr<_Tp>>*)this)->push_back(std::unique_ptr<_Tp>(item));
        }

        return true;
    }
};

} // namespace srlz

#endif // SRLZ_VECTOR_HPP
