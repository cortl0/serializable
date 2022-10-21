/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void nested_custom_entity_test()
{
    using namespace srlz;

    constexpr size_t size = size_t(512);

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

        std::unique_ptr<std::vector<uint32_t>> custom_vector { new std::vector<uint32_t>() };

        virtual bool serialize(
            char* const buffer,
            const size_t buffer_size,
            size_t& buffer_offset
            ) const override
        {
            size_t length = custom_vector->size();

            if (!write(static_cast<const void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_offset))
                return false;

            for (auto& item : *custom_vector)
                if (!write(static_cast<const void* const>(&item), sizeof(item), buffer, buffer_size, buffer_offset))
                    return false;

            return serializable::serialize(buffer, buffer_size, buffer_offset);
        }

        virtual bool deserialize(
            const char* const buffer,
            const size_t buffer_size,
            size_t& buffer_offset
            ) const override
        {
            custom_vector->clear();

            size_t length;

            if (!read(static_cast<void* const>(&length), sizeof(size_t), buffer, buffer_size, buffer_offset))
                return false;

            for (size_t i = 0; i < length; ++i)
            {
                uint32_t value;

                if (!read(static_cast<void* const>(&value), sizeof(value), buffer, buffer_size, buffer_offset))
                    return false;

                custom_vector->push_back(value);
            }

            return serializable::deserialize(buffer, buffer_size, buffer_offset);
        }
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

    entity first;
    entity second;
    constexpr int32_t test_value = 15;
    first.nested.get_unsafe().i.get_unsafe() = test_value - 5;
    first.nested.get_unsafe().custom_vector->push_back(test_value);
    first.nested.get_unsafe().custom_vector->push_back(test_value + 5);

    constexpr size_t expected_size =
        sizeof(bool) +
        sizeof(size_t) +
        sizeof(first.nested.get().i.get()) * 2 +
        sizeof(bool) + sizeof(*first.nested.get().custom_vector->begin());

    char buffer[expected_size];
    size_t offset;

    assert(first.serialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(second.deserialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(second.nested.get().i.get() == test_value - 5);
    assert((*second.nested.get().custom_vector)[0] == test_value);
    assert((*second.nested.get().custom_vector)[1] == test_value + 5);

    //debug_helper(buffer, serialize_offset);
}
