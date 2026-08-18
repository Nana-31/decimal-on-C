/**
 * @file test_converters.c
 * @brief Unit-тесты для преобразователей типов s21_decimal.
 *
 * Тестируемые функции:
 * - s21_from_int_to_decimal
 * - s21_from_float_to_decimal
 * - s21_from_decimal_to_int
 * - s21_from_decimal_to_float
 *
 * @note Владелец файла: Dev 3 (Специалист по типам)
 * @note Особое внимание: s21_from_float_to_decimal требует разбора IEEE 754
 */

#include <check.h>

#include "../s21_decimal.h"

/* ======================================================================== */
/*                              ТЕСТЫ                                       */
/* ======================================================================== */

// TODO: Dev 3 - добавляйте тесты для конвертеров
// Пример теста:
//
// START_TEST(test_s21_from_int_to_decimal_basic) {
//     s21_decimal result;
//     int status = s21_from_int_to_decimal(42, &result);
//
//     ck_assert_int_eq(status, S21_OK);
//     ck_assert_int_eq(result.bits[0], 42);
//     ck_assert_int_eq(result.bits[1], 0);
//     ck_assert_int_eq(result.bits[2], 0);
//     ck_assert_int_eq(result.bits[3], 0);  // scale=0, sign=0
// }
// END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

/**
 * @brief Создает тестовый сьют для преобразователей типов.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_converters_suite(void)
{
    Suite *s = suite_create("s21_converters");
    TCase *tc_core = tcase_create("Core");

    // TODO: Dev 3 - добавляйте тесты здесь через tcase_add_test
    // Пример:
    // tcase_add_test(tc_core, test_s21_from_int_to_decimal_basic);

    suite_add_tcase(s, tc_core);
    return s;
}