/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void selective_test()
{
    using namespace srlz;

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
