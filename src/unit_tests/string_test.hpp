/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/string.hpp"

void string_test()
{
    using namespace std::string_literals;

    using namespace srlz;

    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<string, member_type::SRLZ> str;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&str)
        };
    };

    constexpr size_t length = 128;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;
    string test_str_value;
    //!!
    test_str_value.set("text"s);
    first.str.set(test_str_value);
    //!!
    first.str.get_unsafe().set("text"s);
    const size_t expected_size = sizeof(bool) + sizeof(size_t) + test_str_value.length();

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert(first.str.get() == test_str_value);
    assert(first.str.get() == second.str.get());
    assert(second.str.get() == "text"s);

    //debug_helper(buffer, serialize_offset);
}
