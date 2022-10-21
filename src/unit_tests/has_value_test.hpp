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

        member<int32_t, member_type::INT_32> i;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i)
        };
    };

    {
        entity obj;
        assert(obj.i.has_value());
        obj.i.set_has_value(false);
        assert(!obj.i.has_value());
        obj.i.set_has_value(true);
        assert(obj.i.has_value());
    }

    {
        entity obj;
        obj.i.set_has_value(false);
        bool success = false;
        try
        {
            obj.i.get();
        }
        catch (...)
        {
            success = true;
        }
        assert(success);
    }

    {
        entity obj;
        obj.i.set_has_value(false);
        bool success = false;
        try
        {
            obj.i.get_unsafe();
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
            obj.i.get();
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
            obj.i.get_unsafe();
        }
        catch (...)
        {
            success = false;
        }
        assert(success);
    }

    {
        entity obj;
        obj.i.set_has_value(false);
        obj.i.set(15);
        assert(obj.i.has_value());
    }
}
