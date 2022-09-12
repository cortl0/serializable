/**
 * @file serializable_test.cpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable unit tests
 * @version 0.1
 * @date 2022-09-11
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>
#include <iostream>
#include "serializable.hpp"

using namespace srlz;

void debug_helper(char* buffer, size_t length);
void has_value_test();
void simple_serialize_deserialize_test();
void selective_serialization_test();
void all_fundamental_types_test();
void std_string_type_test();
void nested_entity_test();

int main(int argc, char* argv[])
{
    has_value_test();
    simple_serialize_deserialize_test();
    selective_serialization_test();
    all_fundamental_types_test();
    std_string_type_test();
    nested_entity_test();

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
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

    first.i8.set(test_int8_t_value);

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    constexpr size_t expected_size = sizeof(bool) + sizeof(int8_t);
    assert(expected_size == serialize_shift);
    assert(expected_size == deserialize_shift);
    assert(test_int8_t_value == first.i8.get());
    assert(test_int8_t_value == second.i8.get());

    //debug_helper(buffer, serialize_shift);
}

void selective_serialization_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member< int8_t     , member_type::INT_8       > i8   ;
        member< int16_t    , member_type::INT_16      > i16  ;
        member< int32_t    , member_type::INT_32      > i32  ;
        member< int64_t    , member_type::INT_64      > i64  ;

        serializable::member_vector_type member_vector =
        {
            static_cast< void* >( &i8   ),
            static_cast< void* >( &i16  ),
            static_cast< void* >( &i32  ),
            static_cast< void* >( &i64  ),
        };
    };

    constexpr size_t length = 128;
    char buffer[length];
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

    first.i8   .set( int8_t(1)   );
    first.i16  .set( int16_t(2)  );
    first.i32  .set( 3L          );
    first.i64  .set( 4LL         );

    first.i8.set_has_value(false);
    first.i32.set_has_value(false);

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    constexpr size_t expected_size = sizeof(bool) * 4 + sizeof(int16_t) + sizeof(int64_t);
    assert(serialize_shift == expected_size);
    assert(serialize_shift == deserialize_shift);
    assert(!second.i8.has_value());
    assert(!second.i32.has_value());
    assert(first.i16.get() == second.i16.get());
    assert(first.i64.get() == second.i64.get());

    //debug_helper(buffer, serialize_shift);
}

void all_fundamental_types_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

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
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

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

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));

    constexpr size_t expected_size = 
        sizeof(bool)    * 11 +
        sizeof(int8_t)  *  2 +
        sizeof(int16_t) *  2 +
        sizeof(int32_t) *  2 +
        sizeof(int64_t) *  2 +
        sizeof(float)        +
        sizeof(double)       +
        sizeof(long double);

    assert(expected_size == serialize_shift);
    assert(expected_size == deserialize_shift);
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

    //debug_helper(buffer, serialize_shift);
}

void std_string_type_test()
{
    class entity final : public serializable
    {
    public:
        virtual ~entity() = default;
        entity() : serializable(member_vector) {}

        member<std::string, member_type::STD_STRING> str;

        serializable::member_vector_type member_vector =
        {
            static_cast<void*>(&str)
        };
    };

    constexpr size_t length = 128;
    char buffer[length];
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;
    const std::string test_str_value = "text";

    first.str.set(test_str_value);

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    const size_t expected_size = sizeof(bool) + sizeof(size_t) + test_str_value.length();
    assert(expected_size == serialize_shift);
    assert(expected_size == deserialize_shift);
    assert(first.str.get() == test_str_value);
    assert(first.str.get() == second.str.get());

    //debug_helper(buffer, serialize_shift);
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
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

    first.nested.get_unsafe().i8.set(test_int8_t_value);

    assert(first.nested.get().i8.get() == test_int8_t_value);
    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    const size_t expected_size = sizeof(bool) + sizeof(bool) + sizeof(int8_t);
    assert(expected_size == serialize_shift);
    assert(expected_size == deserialize_shift);
    assert(test_int8_t_value == first.nested.get().i8.get());
    assert(test_int8_t_value == second.nested.get().i8.get());

    //debug_helper(buffer, serialize_shift);
}
