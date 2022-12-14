/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_SERIALIZABLE_HPP
#define SRLZ_SERIALIZABLE_HPP

#include <cassert>
#include <cstring>
#include <vector>

#include "member.hpp"
#include "memory.h"
#include "base.hpp"

namespace srlz
{

class serializable : public base
{
public:
    using member_vector_type = const std::vector<void*>;

    virtual ~serializable() = default;

    serializable(member_vector_type& member_vector)
        : member_vector(member_vector) {}

    serializable& operator=(const serializable& other)
    {
#define SRLZ_COPY_FUNDAMENTAL_TYPE(T, member_type) \
        auto& mem = *static_cast<const member<T, member_type>*>(member_vector[i]); \
        auto& mem_other = *static_cast<const member<T, member_type>*>(other.member_vector[i]); \
        *mem.value_.get() = *mem_other.value_.get();
// SRLZ_COPY_FUNDAMENTAL_TYPE

        for (size_t i = 0; i < member_vector.size(); ++i)
        {
            auto& common = *static_cast<member<int8_t, member_type::COMMON>*>(member_vector[i]);
            auto& common_other = *static_cast<const member<int8_t, member_type::COMMON>*>(other.member_vector[i]);

            common.has_value_ = common_other.has_value_;

            if (!common.has_value_)
                continue;

            switch (common.get_type())
            {
            case member_type::BOOL        : { SRLZ_COPY_FUNDAMENTAL_TYPE( bool        , member_type::BOOL        ) break; }
            case member_type::INT_8       : { SRLZ_COPY_FUNDAMENTAL_TYPE( int8_t      , member_type::INT_8       ) break; }
            case member_type::INT_16      : { SRLZ_COPY_FUNDAMENTAL_TYPE( int16_t     , member_type::INT_16      ) break; }
            case member_type::INT_32      : { SRLZ_COPY_FUNDAMENTAL_TYPE( int32_t     , member_type::INT_32      ) break; }
            case member_type::INT_64      : { SRLZ_COPY_FUNDAMENTAL_TYPE( int64_t     , member_type::INT_64      ) break; }
            case member_type::U_INT_8     : { SRLZ_COPY_FUNDAMENTAL_TYPE( uint8_t     , member_type::U_INT_8     ) break; }
            case member_type::U_INT_16    : { SRLZ_COPY_FUNDAMENTAL_TYPE( uint16_t    , member_type::U_INT_16    ) break; }
            case member_type::U_INT_32    : { SRLZ_COPY_FUNDAMENTAL_TYPE( uint32_t    , member_type::U_INT_32    ) break; }
            case member_type::U_INT_64    : { SRLZ_COPY_FUNDAMENTAL_TYPE( uint64_t    , member_type::U_INT_64    ) break; }
            case member_type::FLOAT       : { SRLZ_COPY_FUNDAMENTAL_TYPE( float       , member_type::FLOAT       ) break; }
            case member_type::DOUBLE      : { SRLZ_COPY_FUNDAMENTAL_TYPE( double      , member_type::DOUBLE      ) break; }
            case member_type::LONG_DOUBLE : { SRLZ_COPY_FUNDAMENTAL_TYPE( long double , member_type::LONG_DOUBLE ) break; }

            case member_type::SRLZ:
            {
                auto& mem = *static_cast<member<serializable, member_type::SRLZ>*>(member_vector[i]);
                auto& mem_other = *static_cast<const member<serializable, member_type::SRLZ>*>(other.member_vector[i]);
                *mem.value_.get() = *mem_other.value_.get();

                break;
            }

            default:
                assert(false);

                break;
            }
        }

#undef SRLZ_COPY_FUNDAMENTAL_TYPE

        return *this;
    }

    virtual bool serialize(
        char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {

#define SRLZ_SERIALIZE_FUNDAMENTAL_TYPE(T, member_type) \
    auto& mem = *static_cast<const member<T, member_type>*>(memb); \
    if (!write(static_cast<const void* const>(mem.value_.get()), sizeof(*mem.value_.get()), buffer, buffer_size, buffer_offset)) \
        return false;
// SRLZ_SERIALIZE_FUNDAMENTAL_TYPE

        for (auto memb : member_vector)
        {
            auto& common = *static_cast<const member<int8_t, member_type::COMMON>*>(memb);

            if (!write(static_cast<const void* const>(&common.has_value_), sizeof(bool), buffer, buffer_size, buffer_offset))
                return false;

            if (!common.has_value_)
                continue;

            switch (common.get_type())
            {
            case member_type::BOOL        : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( bool        , member_type::BOOL        ) break; }
            case member_type::INT_8       : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( int8_t      , member_type::INT_8       ) break; }
            case member_type::INT_16      : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( int16_t     , member_type::INT_16      ) break; }
            case member_type::INT_32      : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( int32_t     , member_type::INT_32      ) break; }
            case member_type::INT_64      : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( int64_t     , member_type::INT_64      ) break; }
            case member_type::U_INT_8     : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( uint8_t     , member_type::U_INT_8     ) break; }
            case member_type::U_INT_16    : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( uint16_t    , member_type::U_INT_16    ) break; }
            case member_type::U_INT_32    : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( uint32_t    , member_type::U_INT_32    ) break; }
            case member_type::U_INT_64    : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( uint64_t    , member_type::U_INT_64    ) break; }
            case member_type::FLOAT       : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( float       , member_type::FLOAT       ) break; }
            case member_type::DOUBLE      : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( double      , member_type::DOUBLE      ) break; }
            case member_type::LONG_DOUBLE : { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE( long double , member_type::LONG_DOUBLE ) break; }

            case member_type::SRLZ:
            {
                auto& mem = *static_cast<member<serializable, member_type::SRLZ>*>(memb);

                if (!mem.value_->serialize(buffer, buffer_size, buffer_offset))
                    return false;

                break;
            }

            default:
                assert(false);

                return false;
            }
        }

#undef SRLZ_SERIALIZE_FUNDAMENTAL_TYPE

        return true;
    }

    virtual bool deserialize(
        const char* const buffer,
        const size_t buffer_size,
        size_t& buffer_offset
        ) const override
    {

#define SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE(T, member_type) \
    auto& mem = *static_cast<const member<T, member_type>*>(memb); \
    if (!read(static_cast<void* const>(mem.value_.get()), sizeof(*mem.value_.get()), buffer, buffer_size, buffer_offset)) \
        return false;
// SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE

        for (auto memb : member_vector)
        {
            auto& common = *static_cast<member<int8_t, member_type::COMMON>*>(memb);
            if (!read(static_cast<void* const>(&common.has_value_), sizeof(bool), buffer, buffer_size, buffer_offset))
                return false;

            if (!common.has_value_)
                continue;

            switch (common.get_type())
            {
            case member_type::BOOL        : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( bool        , member_type::BOOL        ) break; }
            case member_type::INT_8       : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( int8_t      , member_type::INT_8       ) break; }
            case member_type::INT_16      : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( int16_t     , member_type::INT_16      ) break; }
            case member_type::INT_32      : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( int32_t     , member_type::INT_32      ) break; }
            case member_type::INT_64      : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( int64_t     , member_type::INT_64      ) break; }
            case member_type::U_INT_8     : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( uint8_t     , member_type::U_INT_8     ) break; }
            case member_type::U_INT_16    : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( uint16_t    , member_type::U_INT_16    ) break; }
            case member_type::U_INT_32    : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( uint32_t    , member_type::U_INT_32    ) break; }
            case member_type::U_INT_64    : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( uint64_t    , member_type::U_INT_64    ) break; }
            case member_type::FLOAT       : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( float       , member_type::FLOAT       ) break; }
            case member_type::DOUBLE      : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( double      , member_type::DOUBLE      ) break; }
            case member_type::LONG_DOUBLE : { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE( long double , member_type::LONG_DOUBLE ) break; }

            case member_type::SRLZ:
            {
                auto& mem = *static_cast<member<serializable, member_type::SRLZ>*>(memb);

                if (!mem.value_->deserialize(buffer, buffer_size, buffer_offset))
                    return false;

                break;
            }

            default:
                assert(false);
                
                return false;
            }
        }

#undef SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE

        return true;
    }

private:
    member_vector_type& member_vector;
};

} // namespace srlz

#endif // SRLZ_SERIALIZABLE_HPP
