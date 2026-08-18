/**
 * @file test_comparison.c
 * @brief Unit-тесты для операторов сравнения s21_decimal.
 *
 * Тестируемые функции:
 * - s21_is_less
 * - s21_is_less_or_equal
 * - s21_is_greater
 * - s21_is_greater_or_equal
 * - s21_is_equal
 * - s21_is_not_equal
 *
 * @note Владелец файла: Dev 2 (Логик)
 */

#include <check.h>

#include "../s21_decimal.h"

/* ======================================================================== */
/*                              ТЕСТЫ                                       */
/* ======================================================================== */

// TODO: Dev 2 - добавляйте тесты для операторов сравнения
// Пример теста:
//
// START_TEST(test_s21_is_less_basic) {
//     s21_decimal a = {{10, 0, 0, 0}};  // 10
//     s21_decimal b = {{20, 0, 0, 0}};  // 20
//
//     ck_assert_int_eq(s21_is_less(a, b), 1);  // 10 < 20 = TRUE
//     ck_assert_int_eq(s21_is_less(b, a), 0);  // 20 < 10 = FALSE
// }
// END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

/**
 * @brief Создает тестовый сьют для операторов сравнения.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_comparison_suite(void)
{
    Suite *s = suite_create("s21_comparison");
    TCase *tc_core = tcase_create("Core");

    // TODO: Dev 2 - добавляйте тесты здесь через tcase_add_test
    // Пример:
    // tcase_add_test(tc_core, test_s21_is_less_basic);

    suite_add_tcase(s, tc_core);
    return s;
}