/**
 * @file serializable.hpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable
 * @version 0.1
 * @date 2022-09-11
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_SERIALIZABLE_H
#define SRLZ_SERIALIZABLE_H

#include <cassert>
#include <cstring>
#include <type_traits>
#include <vector>

namespace srlz
{

enum class member_type : uint8_t
{
    COMMON,
    INT_8,
    INT_16,
    INT_32,
    INT_64,
    U_INT_8,
    U_INT_16,
    U_INT_32,
    U_INT_64,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    STD_STRING,
    SRLZ
};

template<class T, member_type mt>
class member
{
public:
    friend class serializable;

    /**
     * @brief first you need to check if the value exists by calling has_value()
     */
    const T& get() const
    {
        if (!has_value_)
            throw -1;

        return value_;
    }

    /**
     * @brief first you need to check if the value exists by calling has_value()
     */
    T& get_unsafe()
    {
        if (!has_value_)
            throw -1;

        return value_;
    }

    void set(T& value) noexcept
    {
        has_value_ = true;
        value_ = value;
    }

    void set(const T& value) noexcept
    {
        has_value_ = true;
        value_ = value;
    }

    const bool& has_value() const noexcept
    {
        return has_value_;
    }

    void set_has_value(bool value) noexcept
    {
        has_value_ = value;
    }

private:
    const member_type type_ = mt;
    bool has_value_ = true;
    T value_;
};

class serializable
{
public:

    using member_vector_type = const std::vector<void*>;

    virtual ~serializable() = default;

    serializable(member_vector_type& member_vector)
        : member_vector(member_vector) {}

    bool serialize(char* const buffer, const size_t size, size_t& shift) const
    {
        auto write = [&buffer, &size, &shift](const void* const value, const size_t length) -> bool
        {
            if (shift + length > size)
                return false;

            std::memcpy(buffer + shift, value, length);
            shift += length;

            return true;
        };

#define SRLZ_SERIALIZE_FUNDAMENTAL_TYPE(T, member_type) \
    auto& mem = *static_cast<const member<T, member_type>*>(memb); \
    if (!write(static_cast<const void* const>(&mem.value_), sizeof(mem.value_))) \
        return false;
// SRLZ_SERIALIZE_FUNDAMENTAL_TYPE

        for (auto memb : member_vector)
        {
            auto& common = *static_cast<const member<int8_t, member_type::COMMON>*>(memb);

            if (!write(static_cast<const void* const>(&common.has_value_), sizeof(bool)))
                return false;

            if (!common.has_value_)
                continue;

            switch (*static_cast<member_type*>(memb))
            {
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

            case member_type::STD_STRING:
            {
                auto& mem = *static_cast<member<std::string, member_type::STD_STRING>*>(memb);

                const size_t length = mem.value_.length();

                if (!write(static_cast<const void* const>(&length), sizeof(size_t)))
                    return false;

                if (!write(static_cast<const void* const>(mem.value_.c_str()), length))
                    return false;

                break;
            }

            case member_type::SRLZ:
            {
                auto& mem = *static_cast<member<serializable, member_type::SRLZ>*>(memb);

                if (!mem.value_.serialize(buffer, size, shift))
                    return false;

                break;
            }

            default:
                assert(false);

                return false;
            }
        }

        return true;
    }
    bool deserialize(const char* const buffer, const size_t size, size_t& shift) const
    {
        auto read = [&buffer, &size, &shift](void* const value, const size_t length) -> bool
        {
            if (shift + length > size)
                return false;

            std::memcpy(value, buffer + shift, length);
            shift += length;

            return true;
        };

#define SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE(T, member_type) \
    auto& mem = *static_cast<member<T, member_type>*>(memb); \
    if (!read(static_cast<void* const>(&mem.value_), sizeof(mem.value_))) \
        return false;
// SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE

        for (auto memb : member_vector)
        {
            auto& common = *static_cast<member<int8_t, member_type::COMMON>*>(memb);
            if (!read(static_cast<void* const>(&common.has_value_), sizeof(bool)))
                return false;

            if (!common.has_value_)
                continue;

            switch (*static_cast<member_type*>(memb))
            {
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

            case member_type::STD_STRING:
            {
                auto& mem = *static_cast<member<std::string, member_type::STD_STRING>*>(memb);

                size_t length;
                    
                if (!read(static_cast<void* const>(&length), sizeof(size_t)))
                    return false;

                mem.value_.resize(length);

                if (!read(static_cast<void* const>(mem.value_.data()), length))
                    return false;

                break;
            }

            case member_type::SRLZ:
            {
                auto& mem = *static_cast<member<serializable, member_type::SRLZ>*>(memb);

                if (!mem.value_.deserialize(buffer, size, shift))
                    return false;

                break;
            }

            default:
                assert(false);
                
                return false;
            }
        }

        return true;
    }

private:
    member_vector_type& member_vector;
    inline static const char null_terminator = '\0';
};

} // namespace srlz

#endif // SRLZ_SERIALIZABLE_H
