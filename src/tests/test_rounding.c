/**
 * @file test_rounding.c
 * @brief Unit-тесты для функций округления и смены знака (negate, floor, round,
 * truncate).
 */

#include <check.h>

#include "../s21_decimal.h"
#include "../s21_helpers.h"

/* ======================================================================== */
/*                           ТЕСТЫ S21_NEGATE                               */
/* ======================================================================== */

/**
 * @brief Тест смены знака положительного числа.
 */
START_TEST(test_s21_negate_positive) {
  s21_decimal a = {{123, 0, 0, 0}};
  s21_decimal result;

  int status = s21_negate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 123);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Стал отрицательным
}
END_TEST

/**
 * @brief Тест смены знака отрицательного числа.
 */
START_TEST(test_s21_negate_negative) {
  s21_decimal a = {{456, 0, 0, 0}};
  s21_set_sign(&a, 1);  // Делаем отрицательным
  s21_decimal result;

  int status = s21_negate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 456);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Стал положительным
}
END_TEST

/**
 * @brief Тест смены знака нуля (должен остаться положительным).
 */
START_TEST(test_s21_negate_zero) {
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_set_sign(&zero, 1);  // Отрицательный ноль
  s21_decimal result;

  int status = s21_negate(zero, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль всегда положительный
}
END_TEST

/**
 * @brief Тест обработки NULL-указателя.
 */
START_TEST(test_s21_negate_null) {
  s21_decimal a = {{100, 0, 0, 0}};

  ck_assert_int_eq(s21_negate(a, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/**
 * @brief Тест сохранения scale и мантиссы.
 */
START_TEST(test_s21_negate_preserves_scale) {
  s21_decimal a = {{789, 0, 0, 0}};
  s21_set_scale(&a, 5);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_negate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 789);
  ck_assert_int_eq(s21_get_scale(&result), 5);  // Scale не изменился
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Только знак изменился
}
END_TEST

/**
 * @brief Тест двойного применения negate (должно вернуть исходное число).
 */
START_TEST(test_s21_negate_double_application) {
  s21_decimal a = {{12345, 67890, 11111, 0}};
  s21_set_scale(&a, 7);
  s21_set_sign(&a, 1);
  s21_decimal temp;
  s21_decimal result;

  s21_negate(a, &temp);
  s21_negate(temp, &result);

  ck_assert_uint_eq(result.bits[0], a.bits[0]);
  ck_assert_uint_eq(result.bits[1], a.bits[1]);
  ck_assert_uint_eq(result.bits[2], a.bits[2]);
  ck_assert_int_eq(s21_get_sign(&result), s21_get_sign(&a));
  ck_assert_int_eq(s21_get_scale(&result), s21_get_scale(&a));
}
END_TEST

/**
 * @brief Тест работы с многоблочным числом.
 */
START_TEST(test_s21_negate_multi_block) {
  s21_decimal a = {{0x12345678, 0xABCDEF00, 0x11111111, 0}};
  s21_set_scale(&a, 10);
  s21_decimal result;

  int status = s21_negate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0x12345678);
  ck_assert_uint_eq(result.bits[1], 0xABCDEF00);
  ck_assert_uint_eq(result.bits[2], 0x11111111);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Стал отрицательным
  ck_assert_int_eq(s21_get_scale(&result), 10);
}
END_TEST

/**
 * @brief Тест работы с максимальным числом.
 */
START_TEST(test_s21_negate_max_value) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_set_scale(&a, 28);
  s21_decimal result;

  int status = s21_negate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(s21_get_sign(&result), 1);
  ck_assert_int_eq(s21_get_scale(&result), 28);
}
END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

/**
 * @brief Создание и настройка сьюта для функций округления и negate.
 */
Suite *s21_rounding_suite(void) {
  Suite *s = suite_create("s21_rounding");
  TCase *tc_core = tcase_create("Core");

  // Тесты для s21_negate
  tcase_add_test(tc_core, test_s21_negate_positive);
  tcase_add_test(tc_core, test_s21_negate_negative);
  tcase_add_test(tc_core, test_s21_negate_zero);
  tcase_add_test(tc_core, test_s21_negate_null);
  tcase_add_test(tc_core, test_s21_negate_preserves_scale);
  tcase_add_test(tc_core, test_s21_negate_double_application);
  tcase_add_test(tc_core, test_s21_negate_multi_block);
  tcase_add_test(tc_core, test_s21_negate_max_value);

  suite_add_tcase(s, tc_core);
  return s;
}