/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/vector.hpp"

void vector_test()
{
    using namespace srlz;

    class item_entity final : public serializable
    {
    public:
        virtual ~item_entity() = default;
        item_entity() : serializable(member_vector) {}

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

        member<vector<item_entity>, member_type::SRLZ> v;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&v)
        };
    };

    constexpr int32_t test_value = 15;
    entity first;
    entity second;
    first.v.get_unsafe().push_back(std::make_unique<item_entity>());
    first.v.get().back().get()->i.set(test_value);
    first.v.get_unsafe().push_back(std::make_unique<item_entity>());
    first.v.get().back().get()->i.set(test_value + 1);
    const size_t vector_size = first.v.get().size();

    const size_t expected_size =
        sizeof(bool) +
        sizeof(size_t) +
        (sizeof(bool) + sizeof(first.v.get().begin()->get()->i.get())) * vector_size;

    char buffer[expected_size];
    size_t offset;

    assert(first.serialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(second.deserialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(vector_size == first.v.get().size());
    assert(vector_size == second.v.get().size());
    assert(first.v.get().begin()->get()->i.get() == test_value);
    assert(first.v.get().back().get()->i.get() == test_value + 1);
    assert(second.v.get().begin()->get()->i.get() == test_value);
    assert(second.v.get().back().get()->i.get() == test_value + 1);

    //debug_helper(buffer, serialize_offset);
}
