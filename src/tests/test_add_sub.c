/**
 * @file test_add_sub.c
 * @brief Unit-тесты для операций сложения и вычитания.
 */

#include <check.h>

#include "../s21_decimal.h"
#include "../s21_helpers.h"

/* ======================================================================== */
/*                              ТЕСТЫ S21_ADD                               */
/* ======================================================================== */

/**
 * @brief Тест базового сложения чисел с одинаковыми знаками.
 */
START_TEST(test_s21_add_basic_same_sign) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_decimal result;

  int status = s21_add(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 300);
  ck_assert_int_eq(s21_get_sign(&result), 0);
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/**
 * @brief Тест сложения чисел с разными масштабами (scale).
 */
START_TEST(test_s21_add_different_scales) {
  s21_decimal a = {{15, 0, 0, 0}};   // 1.5 (scale=1)
  s21_decimal b = {{225, 0, 0, 0}};  // 2.25 (scale=2)
  s21_set_scale(&a, 1);
  s21_set_scale(&b, 2);
  s21_decimal result;

  int status = s21_add(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 375);  // 1.50 + 2.25 = 3.75
  ck_assert_int_eq(s21_get_scale(&result), 2);
}
END_TEST

/**
 * @brief Тест сложения чисел с разными знаками (фактически вычитание).
 */
START_TEST(test_s21_add_different_signs_subtraction) {
  s21_decimal a = {{500, 0, 0, 0}};  // 5.00 (+)
  s21_decimal b = {{300, 0, 0, 0}};  // 3.00 (-)
  s21_set_scale(&a, 2);
  s21_set_scale(&b, 2);
  s21_set_sign(&b, 1);
  s21_decimal result;

  int status = s21_add(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 200);  // 5.00 - 3.00 = 2.00
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Знак числа с большим модулем
  ck_assert_int_eq(s21_get_scale(&result), 2);
}
END_TEST

/**
 * @brief Тест сложения противоположных чисел (сведение к абсолютному нулю).
 */
START_TEST(test_s21_add_cancel_to_zero) {
  s21_decimal a = {{12345, 67890, 11111, 0}};
  s21_decimal b = a;
  s21_set_sign(&b, 1);
  s21_decimal result;

  int status = s21_add(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль должен быть положительным
}
END_TEST

/**
 * @brief Тест переполнения при сложении максимальных значений.
 */
START_TEST(test_s21_add_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;

  int status = s21_add(a, b, &result);

  ck_assert_int_eq(status, S21_ERROR_TOO_LARGE);
}
END_TEST

/**
 * @brief Тест ошибки нормализации (разница scale слишком велика).
 */
START_TEST(test_s21_add_normalization_error) {
  // Большое число с scale=0 и маленькое с scale=28.
  // При попытке умножить первое на 10 двадцать восемь раз произойдет
  // переполнение.
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_set_scale(&b, 28);
  s21_decimal result;

  int status = s21_add(a, b, &result);

  ck_assert_int_eq(status, S21_ERROR_CALCULATION);
}
END_TEST

/**
 * @brief Тест сложения с нулем.
 */
START_TEST(test_s21_add_with_zero) {
  s21_decimal a = {{123, 0, 0, 0}};
  s21_set_scale(&a, 2);
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_set_scale(&zero, 5);
  s21_set_sign(&zero, 1);  // Отрицательный ноль
  s21_decimal result;

  int status = s21_add(a, zero, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 123000);  // 1.23 + 0 = 1.23000
  ck_assert_int_eq(s21_get_scale(&result), 5);
  ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

/* ======================================================================== */
/*                              ТЕСТЫ S21_SUB                               */
/* ======================================================================== */

/**
 * @brief Тест базового вычитания.
 */
START_TEST(test_s21_sub_basic) {
  s21_decimal a = {{500, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_decimal result;

  int status = s21_sub(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 300);
  ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

/**
 * @brief Тест вычитания, дающего отрицательный результат.
 */
START_TEST(test_s21_sub_negative_result) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{300, 0, 0, 0}};
  s21_decimal result;

  int status = s21_sub(a, b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 200);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Результат отрицательный
}
END_TEST

/**
 * @brief Тест вычитания числа из самого себя.
 */
START_TEST(test_s21_sub_equal_numbers) {
  s21_decimal a = {{999, 888, 777, 0}};
  s21_set_scale(&a, 3);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_sub(a, a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль всегда положительный
}
END_TEST

/**
 * @brief Тест вычитания нуля.
 */
START_TEST(test_s21_sub_zero) {
  s21_decimal a = {{456, 0, 0, 0}};
  s21_set_scale(&a, 2);
  s21_set_sign(&a, 1);  // Отрицательное число
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal result;

  int status = s21_sub(a, zero, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 456);
  ck_assert_int_eq(s21_get_scale(&result), 2);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Знак сохранился
}
END_TEST

/**
 * @brief Тест обработки NULL-указателя результата.
 */
START_TEST(test_s21_sub_null_result) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{100, 0, 0, 0}};

  ck_assert_int_eq(s21_sub(a, b, NULL), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_add(a, b, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

Suite *s21_add_sub_suite(void) {
  Suite *s = suite_create("s21_add_sub");
  TCase *tc_core = tcase_create("Core");

  // Тесты сложения
  tcase_add_test(tc_core, test_s21_add_basic_same_sign);
  tcase_add_test(tc_core, test_s21_add_different_scales);
  tcase_add_test(tc_core, test_s21_add_different_signs_subtraction);
  tcase_add_test(tc_core, test_s21_add_cancel_to_zero);
  tcase_add_test(tc_core, test_s21_add_overflow);
  tcase_add_test(tc_core, test_s21_add_normalization_error);
  tcase_add_test(tc_core, test_s21_add_with_zero);

  // Тесты вычитания
  tcase_add_test(tc_core, test_s21_sub_basic);
  tcase_add_test(tc_core, test_s21_sub_negative_result);
  tcase_add_test(tc_core, test_s21_sub_equal_numbers);
  tcase_add_test(tc_core, test_s21_sub_zero);
  tcase_add_test(tc_core, test_s21_sub_null_result);

  suite_add_tcase(s, tc_core);
  return s;
}