/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#ifndef SRLZ_MEMBER_HPP
#define SRLZ_MEMBER_HPP

#include <memory>

#include "member_type.h"

namespace srlz
{

class serializable;

template<class T, member_type mt>
class member
{
public:
    friend class serializable;

    /**
     * @brief first you need to check if the value exists by calling has_value()
     */
    const T& get() const
    {
        if (!has_value_)
            throw -1;

        return *value_.get();
    }

    /**
     * @brief first you need to check if the value exists by calling has_value()
     */
    T& get_unsafe()
    {
        if (!has_value_)
            throw -1;

        return *value_.get();
    }

    void set(const T& value) noexcept
    {
        has_value_ = true;
        *value_.get() = value;
    }

    const bool& has_value() const noexcept
    {
        return has_value_;
    }

    void set_has_value(bool value) noexcept
    {
        has_value_ = value;
    }

    const member_type& get_type() const noexcept
    {
        return type_;
    }

private:
    const member_type type_ = mt;
    bool has_value_ = true;
    std::unique_ptr<T> value_ { new T };
};

} // namespace srlz

#endif // SRLZ_MEMBER_HPP
