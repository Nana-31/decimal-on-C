/**
 * @file test_mul_div.c
 * @brief Unit-тесты для операций умножения и деления.
 */

#include <check.h>

#include "../s21_decimal.h"
#include "../s21_helpers.h"

/* ======================================================================== */
/*                              ТЕСТЫ S21_MUL                               */
/* ======================================================================== */

/**
 * @brief Тест базового умножения целых чисел.
 */
START_TEST(test_s21_mul_basic) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{20, 0, 0, 0}};
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 200);
  ck_assert_int_eq(s21_get_sign(&result), 0);
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/**
 * @brief Тест умножения чисел с разными scale.
 */
START_TEST(test_s21_mul_different_scales) {
  s21_decimal a = {{15, 0, 0, 0}};   // 1.5 (scale=1)
  s21_decimal b = {{225, 0, 0, 0}};  // 2.25 (scale=2)
  s21_set_scale(&a, 1);
  s21_set_scale(&b, 2);
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 3375);  // 1.5 * 2.25 = 3.375
  ck_assert_int_eq(s21_get_scale(&result), 3);
}
END_TEST

/**
 * @brief Тест умножения чисел с разными знаками.
 */
START_TEST(test_s21_mul_different_signs) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_set_sign(&b, 1);  // Делаем b отрицательным
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 20000);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Результат отрицательный
}
END_TEST

/**
 * @brief Тест умножения двух отрицательных чисел.
 */
START_TEST(test_s21_mul_both_negative) {
  s21_decimal a = {{50, 0, 0, 0}};
  s21_decimal b = {{30, 0, 0, 0}};
  s21_set_sign(&a, 1);
  s21_set_sign(&b, 1);
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 1500);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // (-) * (-) = (+)
}
END_TEST

/**
 * @brief Тест умножения на ноль.
 */
START_TEST(test_s21_mul_by_zero) {
  s21_decimal a = {{12345, 0, 0, 0}};
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal result;

  int status = s21_mul(a, zero, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
}
END_TEST

/**
 * @brief Тест умножения на единицу.
 */
START_TEST(test_s21_mul_by_one) {
  s21_decimal a = {{999, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_set_scale(&a, 2);
  s21_decimal result;

  int status = s21_mul(a, one, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 999);
  ck_assert_int_eq(s21_get_scale(&result), 2);
}
END_TEST

/**
 * @brief Тест умножения, результат которого занимает два блока (многоблочный).
 */
START_TEST(test_s21_mul_multi_block_result) {
  // 1 000 000 * 1 000 000 = 1 000 000 000 000
  // Это число не помещается в 32 бита, оно займет bits[0] и bits[1]
  s21_decimal a = {{1000000, 0, 0, 0}};  // 1.000000 (scale=6)
  s21_decimal b = {{1000000, 0, 0, 0}};  // 1.000000 (scale=6)
  s21_set_scale(&a, 6);
  s21_set_scale(&b, 6);
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  // 1 000 000 000 000 = 0xE8D4A51000
  ck_assert_uint_eq(result.bits[0],
                    3567587328U);  // 0xD4A51000 (младшие 32 бита)
  ck_assert_uint_eq(result.bits[1], 232U);  // 0x000000E8 (старшие 32 бита)
  ck_assert_uint_eq(result.bits[2], 0U);
  ck_assert_int_eq(s21_get_scale(&result), 12);  // 6 + 6 = 12
}
END_TEST

/**
 * @brief Тест многоблочного умножения.
 */
START_TEST(test_s21_mul_multi_block) {
  // 0xFFFFFFFF * 0xFFFFFFFF = 0xFFFFFFFE00000001 (занимает 2 блока)
  s21_decimal a = {{0xFFFFFFFF, 0, 0, 0}};
  s21_decimal b = {{0xFFFFFFFF, 0, 0, 0}};
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0x00000001);
  ck_assert_uint_eq(result.bits[1], 0xFFFFFFFE);
  ck_assert_uint_eq(result.bits[2], 0);
}
END_TEST

/**
 * @brief Тест умножения с максимальным scale.
 */
START_TEST(test_s21_mul_max_scale) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_set_scale(&a, 28);
  s21_set_scale(&b, 0);
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 1);
  ck_assert_int_eq(s21_get_scale(&result), 28);
}
END_TEST

/**
 * @brief Тест переполнения при умножении.
 */
START_TEST(test_s21_mul_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result;

  int status = s21_mul(a, b, &result);

  // Результат не помещается в 96 бит даже после нормализации
  ck_assert_int_eq(status, S21_ERROR_TOO_LARGE);
}
END_TEST

/**
 * @brief Тест обработки NULL-указателя.
 */
START_TEST(test_s21_mul_null_result) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};

  ck_assert_int_eq(s21_mul(a, b, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

Suite *s21_mul_div_suite(void) {
  Suite *s = suite_create("s21_mul_div");
  TCase *tc_core = tcase_create("Core");

  // Тесты умножения
  tcase_add_test(tc_core, test_s21_mul_basic);
  tcase_add_test(tc_core, test_s21_mul_different_scales);
  tcase_add_test(tc_core, test_s21_mul_different_signs);
  tcase_add_test(tc_core, test_s21_mul_both_negative);
  tcase_add_test(tc_core, test_s21_mul_by_zero);
  tcase_add_test(tc_core, test_s21_mul_by_one);
  tcase_add_test(tc_core, test_s21_mul_multi_block_result);
  tcase_add_test(tc_core, test_s21_mul_multi_block);
  tcase_add_test(tc_core, test_s21_mul_max_scale);
  tcase_add_test(tc_core, test_s21_mul_overflow);
  tcase_add_test(tc_core, test_s21_mul_null_result);

  suite_add_tcase(s, tc_core);
  return s;
}