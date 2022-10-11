/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void simple_test()
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

    constexpr int8_t test_int8_t_value = int8_t(15);
    constexpr size_t length = 16;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;
    first.i8.set(test_int8_t_value);
    constexpr size_t expected_size = sizeof(bool) + sizeof(int8_t);

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert(test_int8_t_value == first.i8.get());
    assert(test_int8_t_value == second.i8.get());

    //debug_helper(buffer, serialize_offset);
}
