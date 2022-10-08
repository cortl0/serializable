/**
 * @file serializable_test.cpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable
 * @version 0.1
 * @date 2022-09-11
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "serializable.hpp"
#include "serializable_string.hpp"
#include "serializable_vector.hpp"

using namespace std::string_literals;

using namespace srlz;

void debug_helper(char* buffer, size_t length);
void has_value_test();
void simple_serialize_deserialize_test();
void selective_serialization_test();
void all_fundamental_types_test();
void memory_test();
void std_string_type_test();
void nested_entity_test();
void custom_entity_test();
void nested_custom_entity_test();
void vector_test();

int main(int argc, char* argv[])
{
    has_value_test();
    simple_serialize_deserialize_test();
    selective_serialization_test();
    all_fundamental_types_test();
    memory_test();
    std_string_type_test();
    nested_entity_test();
    custom_entity_test();
    nested_custom_entity_test();
    vector_test();

    return 0;
}

void debug_helper(char* buffer, size_t length)
{
    printf("buffer length: %llu\n", length);

    for (size_t i = 0; i < length; ++i)
        printf("buffer[%llu]\t%hhu\n", i, (uint8_t)*(buffer + i));
}

void has_value_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<int8_t, member_type::INT_8> i8;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i8)
        };
    };

    {
        entity obj;
        assert(obj.i8.has_value());
        obj.i8.set_has_value(false);
        assert(!obj.i8.has_value());
        obj.i8.set_has_value(true);
        assert(obj.i8.has_value());
    }

    {
        entity obj;
        obj.i8.set_has_value(false);
        bool success = false;
        try
        {
            obj.i8.get();
        }
        catch (...)
        {
            success = true;
        }
        assert(success);
    }

    {
        entity obj;
        obj.i8.set_has_value(false);
        bool success = false;
        try
        {
            obj.i8.get_unsafe();
        }
        catch (...)
        {
            success = true;
        }
        assert(success);
    }

    {
        entity obj;
        bool success = true;
        try
        {
            obj.i8.get();
        }
        catch (...)
        {
            success = false;
        }
        assert(success);
    }

    {
        entity obj;
        bool success = true;
        try
        {
            obj.i8.get_unsafe();
        }
        catch (...)
        {
            success = false;
        }
        assert(success);
    }

    {
        constexpr int8_t test_int8_t_value = int8_t(15);
        entity obj;
        obj.i8.set_has_value(false);
        obj.i8.set(test_int8_t_value);
        assert(obj.i8.has_value());
        assert(obj.i8.get() == test_int8_t_value);
        assert(obj.i8.get_unsafe() == test_int8_t_value);
    }
}

void simple_serialize_deserialize_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<int8_t, member_type::INT_8> i8;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i8)
        };
    };

    constexpr int8_t test_int8_t_value = int8_t(15);
    constexpr size_t length = 16;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;
    first.i8.set(test_int8_t_value);
    constexpr size_t expected_size = sizeof(bool) + sizeof(int8_t);

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert(test_int8_t_value == first.i8.get());
    assert(test_int8_t_value == second.i8.get());

    //debug_helper(buffer, serialize_offset);
}

void selective_serialization_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member< int8_t  , member_type::INT_8  > i8  ;
        member< int16_t , member_type::INT_16 > i16 ;
        member< int32_t , member_type::INT_32 > i32 ;
        member< int64_t , member_type::INT_64 > i64 ;

        serializable::member_vector_type member_vector =
        {
            static_cast< void* >( &i8  ),
            static_cast< void* >( &i16 ),
            static_cast< void* >( &i32 ),
            static_cast< void* >( &i64 ),
        };
    };

    constexpr size_t length = 128;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;

    first.i8  .set( int8_t(1)  );
    first.i16 .set( int16_t(2) );
    first.i32 .set( 3L         );
    first.i64 .set( 4LL        );

    first.i8.set_has_value(false);
    first.i32.set_has_value(false);
    constexpr size_t expected_size = sizeof(bool) * 4 + sizeof(int16_t) + sizeof(int64_t);

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(serialize_offset == expected_size);
    assert(serialize_offset == deserialize_offset);
    assert(!second.i8.has_value());
    assert(!second.i32.has_value());
    assert(first.i16.get() == second.i16.get());
    assert(first.i64.get() == second.i64.get());

    //debug_helper(buffer, serialize_offset);
}

void all_fundamental_types_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member< int8_t     , member_type::BOOL        > b    ;
        member< int8_t     , member_type::INT_8       > i8   ;
        member< int16_t    , member_type::INT_16      > i16  ;
        member< int32_t    , member_type::INT_32      > i32  ;
        member< int64_t    , member_type::INT_64      > i64  ;
        member< uint8_t    , member_type::U_INT_8     > ui8  ;
        member< uint16_t   , member_type::U_INT_16    > ui16 ;
        member< uint32_t   , member_type::U_INT_32    > ui32 ;
        member< uint64_t   , member_type::U_INT_64    > ui64 ;
        member< float      , member_type::FLOAT       > f    ;
        member< double     , member_type::DOUBLE      > d    ;
        member< long double, member_type::LONG_DOUBLE > ld   ;

        serializable::member_vector_type member_vector =
        {
            static_cast< void* >( &b    ),
            static_cast< void* >( &i8   ),
            static_cast< void* >( &i16  ),
            static_cast< void* >( &i32  ),
            static_cast< void* >( &i64  ),
            static_cast< void* >( &ui8  ),
            static_cast< void* >( &ui16 ),
            static_cast< void* >( &ui32 ),
            static_cast< void* >( &ui64 ),
            static_cast< void* >( &f    ),
            static_cast< void* >( &d    ),
            static_cast< void* >( &ld   )
        };
    };

    constexpr size_t length = 128;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;

    first.b    .set( true        );
    first.i8   .set( int8_t(1)   );
    first.i16  .set( int16_t(2)  );
    first.i32  .set( 3L          );
    first.i64  .set( 4LL         );
    first.ui8  .set( uint8_t(5)  );
    first.ui16 .set( uint16_t(6) );
    first.ui32 .set( 7LU         );
    first.ui64 .set( 8LLU        );
    first.f    .set( 9.0F        );
    first.d    .set( 10.0        );
    first.ld   .set( 11.0DL      );

    constexpr size_t expected_size = 
        sizeof(bool)    * 12 +
        sizeof(bool)         +
        sizeof(int8_t)  *  2 +
        sizeof(int16_t) *  2 +
        sizeof(int32_t) *  2 +
        sizeof(int64_t) *  2 +
        sizeof(float)        +
        sizeof(double)       +
        sizeof(long double);

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert(first.b    .get() == second.b    .get());
    assert(first.i8   .get() == second.i8   .get());
    assert(first.i16  .get() == second.i16  .get());
    assert(first.i32  .get() == second.i32  .get());
    assert(first.i64  .get() == second.i64  .get());
    assert(first.ui8  .get() == second.ui8  .get());
    assert(first.ui16 .get() == second.ui16 .get());
    assert(first.ui32 .get() == second.ui32 .get());
    assert(first.ui64 .get() == second.ui64 .get());
    assert(first.f    .get() == second.f    .get());
    assert(first.d    .get() == second.d    .get());
    assert(first.ld   .get() == second.ld   .get());

    //debug_helper(buffer, serialize_offset);
}

void memory_test()
{
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

void std_string_type_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<serializable_string, member_type::SRLZ> str;

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
    serializable_string test_str_value;
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

void nested_entity_test()
{
    class nested_entity final : public serializable
    {
    public:
        virtual ~nested_entity() = default;
        nested_entity() : serializable(member_vector) {}

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

        member<nested_entity, member_type::SRLZ> nested;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&nested)
        };
    };

    constexpr int8_t test_int8_t_value = int8_t(15);
    constexpr size_t length = 1024;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;
    first.nested.get_unsafe().i8.set(test_int8_t_value);
    const size_t expected_size = sizeof(bool) + sizeof(bool) + sizeof(int8_t);

    assert(first.nested.get().i8.get() == test_int8_t_value);
    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert(test_int8_t_value == first.nested.get().i8.get());
    assert(test_int8_t_value == second.nested.get().i8.get());

    //debug_helper(buffer, serialize_offset);
}

void custom_entity_test()
{
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

void nested_custom_entity_test()
{
    constexpr size_t size = size_t(512);

    class nested_entity final : public serializable
    {
    public:
        virtual ~nested_entity()
        {
        }

        nested_entity() : serializable(member_vector) {}

        member<int8_t, member_type::INT_8> i8;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&i8)
        };

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

    constexpr int8_t test_int8_t_value = int8_t(15);
    constexpr size_t length = 1024;
    char buffer[length];
    size_t serialize_offset = 0;
    size_t deserialize_offset = 0;
    entity first;
    entity second;
    first.nested.get_unsafe().custom_vector->push_back(test_int8_t_value);
    first.nested.get_unsafe().custom_vector->push_back(test_int8_t_value + 5);
    first.nested.get_unsafe().i8.get_unsafe() = test_int8_t_value - 5;
    size_t expected_size = sizeof(bool) + sizeof(size_t) + sizeof(int8_t) * 2 + sizeof(bool) + sizeof(int8_t);

    assert(first.serialize(buffer, length, serialize_offset));
    assert(second.deserialize(buffer, length, deserialize_offset));
    assert(expected_size == serialize_offset);
    assert(expected_size == deserialize_offset);
    assert((*second.nested.get().custom_vector)[0] == test_int8_t_value);
    assert((*second.nested.get().custom_vector)[1] == test_int8_t_value + 5);
    assert(second.nested.get().i8.get() == test_int8_t_value - 5);

    //debug_helper(buffer, serialize_offset);
}

void vector_test()
{
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

        member<serializable_vector<item_entity>, member_type::SRLZ> v;

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
