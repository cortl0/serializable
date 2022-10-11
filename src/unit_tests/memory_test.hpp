/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void memory_test()
{
    using namespace srlz;

    constexpr size_t size = size_t(512);

    class entity final : public serializable
    {
    public:
        virtual ~entity()
        {
            delete [] m.get_unsafe().pointer;
        }

        entity() : serializable(member_vector)
        {
            m.get_unsafe().size = size;
            m.get_unsafe().pointer = new unsigned char[size];
        }

        member<memory, member_type::MEMORY> m;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&m)
        };
    };

    constexpr size_t length = 1024;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;

    for (size_t i = 0; i < size; ++i)
        first.m.get_unsafe().pointer[i] = i % size_t(first.m.get_unsafe().pointer);

    size_t expected_size = sizeof(bool) + size;

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);

    for (size_t i = 0; i < size; ++i)
        assert(first.m.get().pointer[i] == first.m.get().pointer[i]);

    //debug_helper(buffer, serialize_offset);
}
