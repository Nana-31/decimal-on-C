/**
 * @file test_mul_div.c
 * @brief Unit-тесты для операций умножения и деления.
 */

#include <check.h>

#include "../s21_decimal.h"

Suite *s21_mul_div_suite(void) {
  Suite *s = suite_create("s21_mul_div");
  TCase *tc_core = tcase_create("Core");

  // TODO: Dev 1 (Математик) - добавляйте тесты для mul, div

  suite_add_tcase(s, tc_core);
  return s;
}