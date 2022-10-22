/**
 * @brief project serializable
 * @author Ilya Shishkin (cortl@yandex.ru)
 * @license GPL v3.0
 * @copyright Copyright (c) 2022
 */

#include <iostream>

#include "has_value_test.hpp"
#include "length_buffer_test.hpp"
#include "simple_test.hpp"
#include "selective_test.hpp"
#include "fundamental_types_test.hpp"
#include "memory_test.hpp"
#include "string_test.hpp"
#include "nested_entity_test.hpp"
#include "custom_entity_test.hpp"
#include "nested_custom_entity_test.hpp"
#include "vector_test.hpp"
#include "copy_assignment_operator_test.hpp"

using namespace std::string_view_literals;

int main(int argc, char* argv[])
{
    const std::initializer_list<std::pair<void (*)(), std::string_view>> tests = 
    {
        {has_value_test, "has_value_test"sv},
        {length_buffer_test, "length_buffer_test"sv},
        {simple_test, "simple_test"sv},
        {selective_test, "selective_test"sv},
        {fundamental_types_test, "fundamental_types_test"sv},
        {memory_test, "memory_test"sv},
        {string_test, "string_test"sv},
        {vector_test, "vector_test"sv},
        {custom_entity_test, "custom_entity_test"sv},
        {nested_entity_test, "nested_entity_test"sv},
        {nested_custom_entity_test, "nested_custom_entity_test"sv},
        {copy_assignment_operator_test, "copy_assignment_operator_test"sv},
    };

    for (auto& [test, name] : tests)
    {
        printf("started %s\n", name.data());
        test();
        printf("success %s\n", name.data());
    }

    printf("all tests have been successfully passed\n");

    return 0;
}
