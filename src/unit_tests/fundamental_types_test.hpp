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

    constexpr size_t expected_size =
        sizeof(bool) * 12 +
        sizeof(bool)      +
        sizeof(int8_t)    +
        sizeof(int16_t)   +
        sizeof(int32_t)   +
        sizeof(int64_t)   +
        sizeof(uint8_t)   +
        sizeof(uint16_t)  +
        sizeof(uint32_t)  +
        sizeof(uint64_t)  +
        sizeof(float)     +
        sizeof(double)    +
        sizeof(long double);

    entity first;
    entity second;
    first.b    .set( true        );
    first.i8   .set( int8_t(2)   );
    first.i16  .set( int16_t(3)  );
    first.i32  .set( 4L          );
    first.i64  .set( 5LL         );
    first.ui8  .set( uint8_t(6)  );
    first.ui16 .set( uint16_t(7) );
    first.ui32 .set( 8LU         );
    first.ui64 .set( 9LLU        );
    first.f    .set( 10.0F       );
    first.d    .set( 11.0        );
    first.ld   .set( 12.0DL      );
    char buffer[expected_size];
    size_t offset;

    assert(first.serialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
    assert(second.deserialize(buffer, expected_size, offset = 0));
    assert(expected_size == offset);
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
