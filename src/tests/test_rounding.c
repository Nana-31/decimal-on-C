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
/*                          ТЕСТЫ S21_TRUNCATE                              */
/* ======================================================================== */

/**
 * @brief Тест отбрасывания дробной части положительного числа.
 */
START_TEST(test_s21_truncate_positive) {
  s21_decimal a = {{1234, 0, 0, 0}};  // 12.34 (scale=2)
  s21_set_scale(&a, 2);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 12);  // 12.34 -> 12
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

/**
 * @brief Тест отбрасывания дробной части отрицательного числа.
 */
START_TEST(test_s21_truncate_negative) {
  s21_decimal a = {{5678, 0, 0, 0}};  // -56.78 (scale=2)
  s21_set_scale(&a, 2);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 56);  // -56.78 -> -56
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Знак сохраняется
}
END_TEST

/**
 * @brief Тест числа, которое уже является целым (но с scale=1).
 */
START_TEST(test_s21_truncate_already_integer) {
  s21_decimal a = {{999, 0, 0, 0}};  // 99.9 (scale=1)
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 99);  // 99.9 -> 99
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/**
 * @brief Тест отбрасывания дробной части у нуля.
 */
START_TEST(test_s21_truncate_zero) {
  s21_decimal a = {{0, 0, 0, 0}};
  s21_set_scale(&a, 5);
  s21_set_sign(&a, 1);  // Отрицательный ноль
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль всегда положительный
}
END_TEST

/**
 * @brief Тест обработки NULL-указателя.
 */
START_TEST(test_s21_truncate_null) {
  s21_decimal a = {{100, 0, 0, 0}};
  ck_assert_int_eq(s21_truncate(a, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/**
 * @brief Тест отбрасывания дробной части у числа с максимальным scale (28).
 */
START_TEST(test_s21_truncate_max_scale) {
  s21_decimal a = {{123456789, 0, 0, 0}};
  s21_set_scale(&a, 28);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);  // 0.000000000123456789 -> 0
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль всегда положительный
}
END_TEST

/**
 * @brief Тест отбрасывания дробной части у многоблочного числа.
 */
START_TEST(test_s21_truncate_multi_block) {
  // Число с scale=2, мантисса занимает 3 блока
  s21_decimal a = {{0xFFFFFFFF, 0x12345678, 0x00000001, 0}};
  s21_set_scale(&a, 2);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  // Мантисса делится на 100 (scale=2), результат должен быть корректным
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 1);  // Знак сохраняется
}
END_TEST

/**
 * @brief Тест отрицательного дробного числа, которое при усечении становится
 * нулем.
 */
START_TEST(test_s21_truncate_negative_fraction_to_zero) {
  s21_decimal a = {{5, 0, 0, 0}};  // -0.5 (scale=1)
  s21_set_scale(&a, 1);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result),
                   0);  // Результат усечения -0.x должен быть +0
}
END_TEST

/**
 * @brief Тест усечения максимального возможного значения.
 */
START_TEST(test_s21_truncate_max_value) {
  // Максимальное число с 1 знаком после запятой
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_truncate(a, &result);

  ck_assert_int_eq(status, S21_OK);
  // Мантисса делится на 10, результат должен быть корректным
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/* ======================================================================== */
/*                           ТЕСТЫ S21_ROUND                                */
/* ======================================================================== */

/**
 * @brief Тест округления положительного числа (половина вверх).
 */
START_TEST(test_s21_round_positive_half_up) {
  s21_decimal a = {{15, 0, 0, 0}};  // 1.5
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 2);  // 1.5 -> 2
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/**
 * @brief Тест округления положительного числа (половина вниз).
 */
START_TEST(test_s21_round_positive_half_down) {
  s21_decimal a = {{14, 0, 0, 0}};  // 1.4
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 1);  // 1.4 -> 1
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/**
 * @brief Тест округления отрицательного числа (половина от нуля).
 */
START_TEST(test_s21_round_negative_half_away_from_zero) {
  s21_decimal a = {{15, 0, 0, 0}};  // -1.5
  s21_set_scale(&a, 1);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 2);  // -1.5 -> -2
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

/**
 * @brief Тест округления отрицательного числа (половина к нулю).
 */
START_TEST(test_s21_round_negative_half_towards_zero) {
  s21_decimal a = {{14, 0, 0, 0}};  // -1.4
  s21_set_scale(&a, 1);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 1);  // -1.4 -> -1
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 1);
}
END_TEST

/**
 * @brief Тест округления числа, которое уже является целым.
 */
START_TEST(test_s21_round_already_integer) {
  s21_decimal a = {{50, 0, 0, 0}};  // 5.0
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 5);
  ck_assert_int_eq(s21_get_scale(&result), 0);
}
END_TEST

/**
 * @brief Тест округления дроби, которая становится нулем.
 */
START_TEST(test_s21_round_fraction_to_zero) {
  s21_decimal a = {{4, 0, 0, 0}};  // -0.4
  s21_set_scale(&a, 1);
  s21_set_sign(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль всегда положительный
}
END_TEST

/**
 * @brief Тест обработки NULL-указателя.
 */
START_TEST(test_s21_round_null) {
  s21_decimal a = {{100, 0, 0, 0}};
  ck_assert_int_eq(s21_round(a, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/**
 * @brief Тест округления положительной дроби, которая становится нулем.
 */
START_TEST(test_s21_round_positive_fraction_to_zero) {
  s21_decimal a = {{4, 0, 0, 0}};  // 0.4
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);  // Ноль всегда положительный
}
END_TEST

/**
 * @brief Тест округления с переносом разряда (9.9 -> 10).
 */
START_TEST(test_s21_round_with_carry) {
  s21_decimal a = {{99, 0, 0, 0}};  // 9.9
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 10);  // 9.9 -> 10
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);
}
END_TEST

/**
 * @brief Тест округления многоблочного числа.
 */
START_TEST(test_s21_round_multi_block) {
  // Максимальное 64-битное число с 1 знаком после запятой
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_set_scale(&a, 1);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);

  // 0xFFFFFFFF_FFFFFFFF / 10 = 1844674407370955161.5 → округляется до
  // 1844674407370955162 В hex: 0x199999999999999A
  ck_assert_uint_eq(result.bits[0], 0x9999999A);
  ck_assert_uint_eq(result.bits[1], 0x19999999);
  ck_assert_uint_eq(result.bits[2], 0);  // Старший блок обнулился
}
END_TEST

/**
 * @brief Тест округления числа с максимальным scale (28).
 */
START_TEST(test_s21_round_max_scale) {
  s21_decimal a = {{5, 0, 0, 0}};  // 0.000...05 (28 нулей перед 5)
  s21_set_scale(&a, 28);
  s21_decimal result;

  int status = s21_round(a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);  // Округлится до 0
  ck_assert_int_eq(s21_get_scale(&result), 0);
  ck_assert_int_eq(s21_get_sign(&result), 0);
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

  // Тесты для s21_truncate
  tcase_add_test(tc_core, test_s21_truncate_positive);
  tcase_add_test(tc_core, test_s21_truncate_negative);
  tcase_add_test(tc_core, test_s21_truncate_already_integer);
  tcase_add_test(tc_core, test_s21_truncate_zero);
  tcase_add_test(tc_core, test_s21_truncate_null);
  tcase_add_test(tc_core, test_s21_truncate_max_scale);
  tcase_add_test(tc_core, test_s21_truncate_multi_block);
  tcase_add_test(tc_core, test_s21_truncate_negative_fraction_to_zero);
  tcase_add_test(tc_core, test_s21_truncate_max_value);

  // Тесты для s21_round
  tcase_add_test(tc_core, test_s21_round_positive_half_up);
  tcase_add_test(tc_core, test_s21_round_positive_half_down);
  tcase_add_test(tc_core, test_s21_round_negative_half_away_from_zero);
  tcase_add_test(tc_core, test_s21_round_negative_half_towards_zero);
  tcase_add_test(tc_core, test_s21_round_already_integer);
  tcase_add_test(tc_core, test_s21_round_fraction_to_zero);
  tcase_add_test(tc_core, test_s21_round_null);
  tcase_add_test(tc_core, test_s21_round_positive_fraction_to_zero);
  tcase_add_test(tc_core, test_s21_round_with_carry);
  tcase_add_test(tc_core, test_s21_round_multi_block);
  tcase_add_test(tc_core, test_s21_round_max_scale);

  suite_add_tcase(s, tc_core);
  return s;
}