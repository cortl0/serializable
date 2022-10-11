/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "srlz/serializable.hpp"

void has_value_test()
{
    using namespace srlz;

    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<int8_t, member_type::INT_8> i8;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i8)
        };
    };

    {
        entity obj;
        assert(obj.i8.has_value());
        obj.i8.set_has_value(false);
        assert(!obj.i8.has_value());
        obj.i8.set_has_value(true);
        assert(obj.i8.has_value());
    }

    {
        entity obj;
        obj.i8.set_has_value(false);
        bool success = false;
        try
        {
            obj.i8.get();
        }
        catch (...)
        {
            success = true;
        }
        assert(success);
    }

    {
        entity obj;
        obj.i8.set_has_value(false);
        bool success = false;
        try
        {
            obj.i8.get_unsafe();
        }
        catch (...)
        {
            success = true;
        }
        assert(success);
    }

    {
        entity obj;
        bool success = true;
        try
        {
            obj.i8.get();
        }
        catch (...)
        {
            success = false;
        }
        assert(success);
    }

    {
        entity obj;
        bool success = true;
        try
        {
            obj.i8.get_unsafe();
        }
        catch (...)
        {
            success = false;
        }
        assert(success);
    }

    {
        constexpr int8_t test_int8_t_value = int8_t(15);
        entity obj;
        obj.i8.set_has_value(false);
        obj.i8.set(test_int8_t_value);
        assert(obj.i8.has_value());
        assert(obj.i8.get() == test_int8_t_value);
        assert(obj.i8.get_unsafe() == test_int8_t_value);
    }
}
