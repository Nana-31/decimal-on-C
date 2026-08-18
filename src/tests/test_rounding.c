/**
 * @file test_rounding.c
 * @brief Unit-тесты для функций округления и инвертирования знака.
 */

#include <check.h>

#include "../s21_decimal.h"

Suite *s21_rounding_suite(void) {
  Suite *s = suite_create("s21_rounding");
  TCase *tc_core = tcase_create("Core");

  // TODO: Dev 2 (Логик) - добавляйте тесты для floor, round, truncate, negate

  suite_add_tcase(s, tc_core);
  return s;
}