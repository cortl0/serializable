/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "srlz/serializable.hpp"
#include "srlz/string.hpp"

void copy_assignment_operator_test()
{
    using namespace std::string_literals;

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

        nested_entity& operator=(const nested_entity& other)
        {
            serializable::operator=(other);

            return *this;
        }
    };

    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<int32_t, member_type::INT_32> i;

        member<nested_entity, member_type::SRLZ> nested;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i),
            static_cast<void*>(&nested)
        };

        entity& operator=(const entity& other)
        {
            serializable::operator=(other);

            return *this;
        }
    };

    const std::string std_string{"text"s};
    constexpr int32_t test_value = 15;
    string srlz_string;
    srlz_string.set(std_string);

    entity first;
    entity second;
    first.i.set(test_value);
    first.nested.get_unsafe().i.set(test_value + 5);
    second = first;

    assert(test_value == second.i.get());
    assert(test_value + 5 == second.nested.get().i.get());
}
