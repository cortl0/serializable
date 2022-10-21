/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void nested_entity_test()
{
    using namespace srlz;

    class nested_entity final : public serializable
    {
    public:
        virtual ~nested_entity() = default;
        nested_entity() : serializable(member_vector) {}

        member<int32_t, member_type::INT_32> i;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i)
        };
    };

    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<nested_entity, member_type::SRLZ> nested;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&nested)
        };
    };

    constexpr int32_t test_value = 15;
    entity first;
    entity second;
    first.nested.get_unsafe().i.set(test_value);

    constexpr size_t expected_size =
        sizeof(bool) +
        sizeof(bool) +
        sizeof(first.nested.get().i.get());

    char buffer[expected_size];
    size_t offset;

    assert(first.serialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(second.deserialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(test_value == first.nested.get().i.get());
    assert(test_value == second.nested.get().i.get());

    //debug_helper(buffer, serialize_offset);
}
