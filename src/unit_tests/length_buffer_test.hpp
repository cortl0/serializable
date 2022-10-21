/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "srlz/serializable.hpp"

void length_buffer_test()
{
    using namespace srlz;

    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<int32_t, member_type::INT_32> i32;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i32)
        };
    };

    constexpr size_t expected_size = sizeof(bool) + sizeof(int32_t);
    size_t offset;

    {
        entity first;
        entity second;
        char buffer[expected_size];

        assert(first.serialize(buffer, expected_size, offset = 0));
        assert(second.deserialize(buffer, expected_size, offset = 0));
    }

    {
        entity first;
        char buffer[0];

        assert(!first.serialize(buffer, 0, offset = 0));
    }

    {
        char buffer[expected_size - 1];
        entity first;

        assert(!first.serialize(buffer, expected_size - 1, offset = 0));
    }

    {
        char buffer[expected_size];
        entity first;

        assert(!first.serialize(buffer, expected_size, offset = 1));
    }

    {
        char buffer[expected_size];
        entity first;
        entity second;
        first.serialize(buffer, expected_size, offset = 0);

        assert(!second.deserialize(buffer, expected_size - 1, offset = 0));
    }
}
