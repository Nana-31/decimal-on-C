/**
 * @file test_add_sub.c
 * @brief Unit-тесты для операций сложения и вычитания.
 */

#include <check.h>

#include "../s21_decimal.h"

Suite *s21_add_sub_suite(void) {
  Suite *s = suite_create("s21_add_sub");
  TCase *tc_core = tcase_create("Core");

  // TODO: Тимлид - добавляйте тесты для add, sub

  suite_add_tcase(s, tc_core);
  return s;
}