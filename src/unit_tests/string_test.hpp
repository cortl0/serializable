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

    const std::string std_string{"text"s};

    {
        string first;
        string second;
        first.set(std_string);
        second.set(std_string);
        assert(std_string == first);
        assert(std_string == second);
        assert(first == second);
    }

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

    {
        string srlz_string;
        srlz_string.set(std_string);
        entity first;
        entity second;
        first.str.get_unsafe().set(std_string);
        second.str.set(srlz_string);

        assert(std_string == first.str.get());
        assert(std_string == second.str.get());
        assert(srlz_string == first.str.get());
        assert(srlz_string == second.str.get());
    }

    {
        string srlz_string;
        srlz_string.set(std_string);
        entity first;
        entity second;
        first.str.set(srlz_string);
        const size_t expected_size = sizeof(bool) + sizeof(size_t) + srlz_string.length();
        char buffer[expected_size];
        size_t offset;

        assert(first.serialize(buffer, expected_size, offset = 0));
        assert(expected_size == offset);
        assert(second.deserialize(buffer, expected_size, offset = 0));
        assert(expected_size == offset);
        assert(first.str.get() == srlz_string);
        assert(first.str.get() == second.str.get());
        assert(second.str.get() == std_string);

        //debug_helper(buffer, serialize_offset);
    }
}
