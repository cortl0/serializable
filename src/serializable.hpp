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

#include <cstring>
#include <type_traits>
#include <vector>

namespace srlz
{

enum class property_type : uint8_t
{
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

template<class T, property_type pt>
struct srlz_struct
{
    friend class srlz_base;

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
    
    const property_type type = pt;

private:
    bool has_value_ = true;
    T value_;
};

class srlz_base
{
public:

    using property_vector_type = const std::vector<void*>;
    property_vector_type& property_vector;

    virtual ~srlz_base() = default;
    srlz_base(property_vector_type& property_vector)
        : property_vector(property_vector) {}


    inline static const char null_terminator = '\0';

    bool serialize(char* buffer, const size_t size, size_t& shift)
    {
        auto write = [&buffer, &size, &shift](const void* value, size_t length) -> bool
        {
            if((shift += length) > size)
                return false;

            std::memcpy(buffer, value, length);
            buffer += length;

            return true;
        };

#define SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(T, property, property_type) \
    auto& pro = *static_cast<srlz_struct<T, property_type>*>(property); \
    if (!write(static_cast<const void*>(&pro.has_value_), sizeof(bool))) \
        return false; \
    if (pro.has_value_) \
    { \
        if (!write(static_cast<const void*>(&pro.value_), sizeof(pro.value_))) \
            return false; \
    }

        for (auto prop : property_vector)
        {
            switch (*static_cast<property_type*>(prop))
            {
            case property_type::INT_8:       { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int8_t,      prop, property_type::INT_8      ) break; }
            case property_type::INT_16:      { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int16_t,     prop, property_type::INT_16     ) break; }
            case property_type::INT_32:      { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int32_t,     prop, property_type::INT_32     ) break; }
            case property_type::INT_64:      { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int64_t,     prop, property_type::INT_64     ) break; }
            case property_type::U_INT_8:     { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint8_t,     prop, property_type::U_INT_8    ) break; }
            case property_type::U_INT_16:    { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint16_t,    prop, property_type::U_INT_16   ) break; }
            case property_type::U_INT_32:    { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint32_t,    prop, property_type::U_INT_32   ) break; }
            case property_type::U_INT_64:    { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint64_t,    prop, property_type::U_INT_64   ) break; }
            case property_type::FLOAT:       { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(float,       prop, property_type::FLOAT      ) break; }
            case property_type::DOUBLE:      { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(double,      prop, property_type::DOUBLE     ) break; }
            case property_type::LONG_DOUBLE: { SRLZ_SERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(long double, prop, property_type::LONG_DOUBLE) break; }

            case property_type::STD_STRING:
            {
                auto& pro = *static_cast<srlz_struct<std::string, property_type::STD_STRING>*>(prop);

                if (!write(static_cast<const void*>(&pro.has_value_), sizeof(bool)))
                    return false;

                if (pro.has_value_)
                {
                    size_t length = strlen(pro.value_.data());

                    if (!write(static_cast<void*>(&length), sizeof(size_t)))
                        return false;

                    if (!write(static_cast<void*>(pro.value_.data()), length))
                        return false;
                }

                break;
            }

            case property_type::SRLZ:
            {
                auto& pro = *static_cast<srlz_struct<srlz_base, property_type::SRLZ>*>(prop);

                if (!write(static_cast<const void*>(&pro.has_value()), sizeof(bool)))
                    return false;

                if (pro.has_value_)
                    if (!pro.value_.serialize(buffer, size, shift))
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

    bool deserialize(char* buffer, const size_t size, size_t& shift)
    {
        auto read = [&buffer, &size, &shift](void* value, size_t length) -> bool
        {
            if ((shift += length) > size)
                return false;

            std::memcpy(value, buffer, length);
            buffer += length;

            return true;
        };

#define SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(T, property, property_type) \
    auto& pro = *static_cast<srlz_struct<T, property_type>*>(property); \
    if (!read(static_cast<void*>(&pro.has_value_), sizeof(bool))) \
        return false; \
    if (pro.has_value_) \
    { \
        if (!read(static_cast<void*>(&pro.value_), sizeof(pro.value_))) \
            return false; \
    }

        for (auto prop : property_vector)
        {
            switch (*static_cast<property_type*>(prop))
            {
            case property_type::INT_8:       { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int8_t,      prop, property_type::INT_8)       break; }
            case property_type::INT_16:      { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int16_t,     prop, property_type::INT_16)      break; }
            case property_type::INT_32:      { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int32_t,     prop, property_type::INT_32)      break; }
            case property_type::INT_64:      { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(int64_t,     prop, property_type::INT_64)      break; }
            case property_type::U_INT_8:     { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint8_t,     prop, property_type::U_INT_8)     break; }
            case property_type::U_INT_16:    { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint16_t,    prop, property_type::U_INT_16)    break; }
            case property_type::U_INT_32:    { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint32_t,    prop, property_type::U_INT_32)    break; }
            case property_type::U_INT_64:    { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(uint64_t,    prop, property_type::U_INT_64)    break; }
            case property_type::FLOAT:       { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(float,       prop, property_type::FLOAT)       break; }
            case property_type::DOUBLE:      { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(double,      prop, property_type::DOUBLE)      break; }
            case property_type::LONG_DOUBLE: { SRLZ_DESERIALIZE_FUNDAMENTAL_TYPE_PROPERTY(long double, prop, property_type::LONG_DOUBLE) break; }

            case property_type::STD_STRING:
            {
                auto& pro = *static_cast<srlz_struct<std::string, property_type::STD_STRING>*>(prop);

                if (!read(static_cast<void*>(&pro.has_value_), sizeof(bool)))
                    return false;

                if (pro.has_value_)
                {
                    size_t length;
                    
                    if (!read(static_cast<void*>(&length), sizeof(size_t)))
                        return false;

                    char chars[length + 1];

                    if (!read(static_cast<void*>(chars), length))
                        return false;

                    chars[length] = '\0';

                    pro.value_ = std::string(chars);
                }

                break;
            }

            case property_type::SRLZ:
            {
                auto& pro = *static_cast<srlz_struct<srlz_base, property_type::SRLZ>*>(prop);

                if (!read(static_cast<void*>(&pro.has_value_), sizeof(bool)))
                    return false;

                if (pro.has_value_)
                    if (!pro.value_.deserialize(buffer, size, shift))
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
};

} // namespace srlz

#endif // SRLZ_SERIALIZABLE_H
