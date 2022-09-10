/**
 * @file serializable_test.cpp
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @brief project serializable unit tests
 * @version 0.1
 * @date 2022-09-11
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <iostream>
#include <cassert>
#include "serializable.hpp"

using namespace srlz;

constexpr int8_t test_value = 15;

void debug_helper(char* buffer, size_t length);
void has_value_test();
void simple_serialize_deserialize_test();
void all_fundamental_types_test();
void std_string_type_test();
void nested_entity_test();

int main(int argc, char* argv[])
{
    has_value_test();
    simple_serialize_deserialize_test();
    all_fundamental_types_test();
    std_string_type_test();
    nested_entity_test();
    
    return 0;
}

void debug_helper(char* buffer, size_t length)
{
    printf("buffer length: %llu\n", length);

    for(size_t i = 0; i < length; ++i)
        printf("buffer[%hhu]\t%d\n", i, (uint8_t)*(buffer + i));
}

void has_value_test()
{
    class entity final : public srlz_base
    {
    public:
        virtual ~entity() = default;
        entity() : srlz_base(property_vector) {}

        srlz_struct<int8_t, property_type::INT_8> i8;

        srlz_base::property_vector_type property_vector =
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
        entity obj;
        obj.i8.set_has_value(false);
        obj.i8.set(test_value);
        assert(obj.i8.has_value());
        assert(obj.i8.get() == test_value);
        assert(obj.i8.get_unsafe() == test_value);
    }
}

void simple_serialize_deserialize_test()
{
    class entity final : public srlz_base
    {
    public:
        virtual ~entity() = default;
        entity() : srlz_base(property_vector) {}

        srlz_struct<int8_t, property_type::INT_8> i8;

        srlz_base::property_vector_type property_vector =
        {
            static_cast<void*>(&i8)
        };
    };

    constexpr size_t length = 16;
    char buffer[length];
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

    first.i8.set(test_value);

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    constexpr size_t expected_size = sizeof(bool) + sizeof(int8_t);
    assert(serialize_shift == expected_size);
    assert(serialize_shift == deserialize_shift);
    assert(first.i8.get() == test_value);
    assert(second.i8.get() == test_value);

    //debug_helper(buffer, serialize_shift);
}

void all_fundamental_types_test()
{
    class entity final : public srlz_base
    {
    public:
        virtual ~entity() = default;
        entity() : srlz_base(property_vector) {}

        srlz_struct< int8_t     , property_type::INT_8       > i8   ;
        srlz_struct< int16_t    , property_type::INT_16      > i16  ;
        srlz_struct< int32_t    , property_type::INT_32      > i32  ;
        srlz_struct< int64_t    , property_type::INT_64      > i64  ;
        srlz_struct< uint8_t    , property_type::U_INT_8     > ui8  ;
        srlz_struct< uint16_t   , property_type::U_INT_16    > ui16 ;
        srlz_struct< uint32_t   , property_type::U_INT_32    > ui32 ;
        srlz_struct< uint64_t   , property_type::U_INT_64    > ui64 ;
        srlz_struct< float      , property_type::FLOAT       > f    ;
        srlz_struct< double     , property_type::DOUBLE      > d    ;
        srlz_struct< long double, property_type::LONG_DOUBLE > ld   ;

        srlz_base::property_vector_type property_vector =
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

    first.i8   .set(  1   );
    first.i16  .set(  2   );
    first.i32  .set(  3   );
    first.i64  .set(  4   );
    first.ui8  .set(  5   );
    first.ui16 .set(  6   );
    first.ui32 .set(  7   );
    first.ui64 .set(  8   );
    first.f    .set(  9.0 );
    first.d    .set( 10.0 );
    first.ld   .set( 11.0 );

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    constexpr size_t expected_size = 69;
    assert(serialize_shift == expected_size);
    assert(serialize_shift == deserialize_shift);
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
    class entity final : public srlz_base
    {
    public:
        virtual ~entity() = default;
        entity() : srlz_base(property_vector) {}

        srlz_struct<std::string, property_type::STD_STRING> str;

        srlz_base::property_vector_type property_vector =
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

    first.str.set(test_str_value.c_str());

    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    const size_t expected_size = sizeof(bool) + sizeof(size_t) + strlen(test_str_value.c_str());
    assert(serialize_shift == expected_size);
    assert(serialize_shift == deserialize_shift);
    assert(first.str.get() == test_str_value);
    assert(first.str.get() == second.str.get());

    //debug_helper(buffer, serialize_shift);
}

void nested_entity_test()
{
    class nested_entity final : public srlz_base
    {
    public:
        virtual ~nested_entity() = default;
        nested_entity() : srlz_base(property_vector) {}

        srlz_struct<int8_t, property_type::INT_8> i8;

        srlz_base::property_vector_type property_vector =
        {
            static_cast<void*>(&i8)
        };
    };

    class entity final : public srlz_base
    {
    public:
        virtual ~entity() = default;
        entity() : srlz_base(property_vector) {}

        srlz_struct<nested_entity, property_type::SRLZ> nested;

        srlz_base::property_vector_type property_vector =
        {
            static_cast<void*>(&nested)
        };
    };

    constexpr size_t length = 1024;
    char buffer[length];
    size_t serialize_shift = 0;
    size_t deserialize_shift = 0;
    entity first;
    entity second;

    first.nested.get_unsafe().i8.set(test_value);

    assert(first.nested.get().i8.get() == test_value);
    assert(first.serialize(buffer, length, serialize_shift));
    assert(second.deserialize(buffer, length, deserialize_shift));
    const size_t expected_size = sizeof(bool) + sizeof(bool) + sizeof(int8_t);
    assert(serialize_shift == expected_size);
    assert(serialize_shift == deserialize_shift);
    assert(first.nested.get().i8.get() == second.nested.get().i8.get());

    //debug_helper(buffer, serialize_shift);
}
