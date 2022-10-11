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

        member<int8_t, member_type::INT_8> i8;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i8)
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

    constexpr int8_t test_int8_t_value = int8_t(15);
    constexpr size_t length = 512;
    char buffer[length];
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

    first.v.get_unsafe().push_back(std::make_unique<item_entity>());
    first.v.get().back().get()->i8.set(88);
    first.v.get_unsafe().push_back(std::make_unique<item_entity>());
    first.v.get().back().get()->i8.set(89);

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));

    size_t expected_size = sizeof(bool) + sizeof(size_t) + (sizeof(bool) + sizeof(int8_t)) * 2;
    assert(expected_size == serialize_shift);
    assert(expected_size == deserialize_shift);
    assert(2 == first.v.get().size());
    assert(2 == second.v.get().size());
    assert(first.v.get().begin()->get()->i8.get() == 88);
    assert(first.v.get().back().get()->i8.get() == 89);
    assert(second.v.get().begin()->get()->i8.get() == 88);
    assert(second.v.get().back().get()->i8.get() == 89);

    //debug_helper(buffer, serialize_shift);
}
