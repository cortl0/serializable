/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>
#include <memory>
#include <vector>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void custom_entity_test()
{
    using namespace srlz;

    constexpr size_t size = size_t(512);

    class entity final : public serializable
    {
    public:
        virtual ~entity()
        {
        }

        entity() : serializable(member_vector) {}

        serializable::member_vector_type member_vector;

        std::unique_ptr<std::vector<uint8_t>> custom_vector { new std::vector<uint8_t>() };

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
                uint8_t value;

                if (!read(static_cast<void* const>(&value), sizeof(value), buffer, buffer_size, buffer_offset))
                    return false;

                custom_vector->push_back(value);
            }

            return serializable::deserialize(buffer, buffer_size, buffer_offset);
        }

    };

    constexpr int8_t test_int8_t_value = int8_t(15);
    constexpr size_t length = 1024;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;
    first.custom_vector->push_back(test_int8_t_value);
    first.custom_vector->push_back(test_int8_t_value + 5);
    size_t expected_size = sizeof(size_t) + sizeof(int8_t) * 2;

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert((*second.custom_vector)[0] == test_int8_t_value);
    assert((*second.custom_vector)[1] == test_int8_t_value + 5);

    //debug_helper(buffer, serialize_offset);
}