/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_MEMBER_TYPE_H
#define SRLZ_MEMBER_TYPE_H

#include <type_traits>

namespace srlz
{

enum class member_type : uint8_t
{
    COMMON,

    BOOL,
    INT_8,
    INT_16,
    INT_32,
    INT_64,
    U_INT_8,
    U_INT_16,
    U_INT_32,
    U_INT_64,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    
    MEMORY,
    SRLZ,
};

} // namespace srlz

#endif // SRLZ_MEMBER_TYPE_H
