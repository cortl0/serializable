/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <cassert>

#include "debug_helper.hpp"
#include "srlz/serializable.hpp"

void fundamental_types_test()
{
    using namespace srlz;

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
