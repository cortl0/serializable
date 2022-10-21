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

    constexpr size_t size = 512ULL;

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

        member<memory, member_type::SRLZ> m;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&m)
        };
    };

    entity first;
    entity second;

    for (size_t i = 0; i < size; ++i)
        first.m.get_unsafe().pointer[i] = i % size_t(first.m.get_unsafe().pointer);

    constexpr size_t expected_size = sizeof(bool) + sizeof(size_t) + size;
    char buffer[expected_size];
    size_t offset;

    assert(first.serialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(second.deserialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);

    for (size_t i = 0; i < size; ++i)
        assert(first.m.get().pointer[i] == first.m.get().pointer[i]);

    //debug_helper(buffer, serialize_offset);
}
