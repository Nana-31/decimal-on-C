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
#include <limits.h>
#include <math.h>

#include "../s21_decimal.h"

/* ======================================================================== */
/*                              ТЕСТЫ ДЛЯ КОНВЕРТАЦИИ ИЗ INT В DECIMAL */
/* ======================================================================== */

START_TEST(test_s21_from_int_to_decimal_basic) {
  s21_decimal result;
  int status = s21_from_int_to_decimal(55, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 55);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_negative) {
  s21_decimal result;
  int status = s21_from_int_to_decimal(-77, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 77);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], (1U << 31));
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_zero) {
  s21_decimal result;
  int status = s21_from_int_to_decimal(0, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_max) {
  s21_decimal result;
  int status = s21_from_int_to_decimal(INT_MAX, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 2147483647);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_min) {
  s21_decimal result;
  int status = s21_from_int_to_decimal(INT_MIN, &result);
  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 2147483648U);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], (1U << 31));
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_null) {
  int status = s21_from_int_to_decimal(10, NULL);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

/* ======================================================================== */
/*                              ТЕСТЫ ДЛЯ КОНВЕРТАЦИИ ИЗ DECIMAL В INT */
/* ======================================================================== */

START_TEST(test_s21_from_decimal_to_int_basic) {
  s21_decimal src;
  src.bits[0] = 123;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0;

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result, 123);
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_truncate) {
  s21_decimal src;
  src.bits[0] = 12345;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = (2 << 16);  // Записали scale = 2 в биты 16-23

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result, 123);  // Дробь должна отвалиться
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_truncate_negative) {
  s21_decimal src;
  src.bits[0] = 555;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = (1 << 16) | (1U << 31);  // scale = 1, знак минус: -55.5

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result, -55);  // Усечение к нулю, не к -56
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_min) {
  s21_decimal src;
  src.bits[0] = (int)2147483648U;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = (1U << 31);  // Знак минус

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result, INT_MIN);  // Должно стать ровно -2147483648
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_null) {
  s21_decimal src;
  src.bits[0] = 10;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0;

  int status = s21_from_decimal_to_int(src, NULL);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_overflow_positive) {
  s21_decimal src;
  src.bits[0] = (int)2147483648U;  // INT_MAX + 1, знак плюс
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0;

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_overflow_negative) {
  s21_decimal src;
  src.bits[0] = (int)2147483649U;  // На 1 больше допустимого по модулю
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = (1U << 31);  // Знак минус

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_bits_overflow) {
  s21_decimal src;
  src.bits[0] = 1;
  src.bits[1] = 1;  // Задействован bits[1] -> число точно не влезает в int
  src.bits[2] = 0;
  src.bits[3] = 0;

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

/* ======================================================================== */
/*                              ТЕСТЫ ДЛЯ КОНВЕРТАЦИИ ИЗ DECIMAL В FLOAT */
/* ======================================================================== */

START_TEST(test_s21_from_decimal_to_float_basic) {
  s21_decimal src = {{123, 0, 0, 0}};
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_float_eq_tol(result, 123.0f, 1e-4);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_decimal_part) {
  s21_decimal src = {{1234, 0, 0, (2 << 16) | (1U << 31)}};
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_float_eq_tol(result, -12.34f,
                         1e-4);  // Заведомо проходит округление
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_null) {
  s21_decimal src = {{42, 0, 0, 0}};
  int status = s21_from_decimal_to_float(src, NULL);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_invalid_scale) {
  s21_decimal src = {{100, 0, 0, (29 << 16)}};
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_large_mantissa) {
  s21_decimal src = {{0, 1, 0, 0}};  // Включаем только bits[1] -> это 2^32
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_float_eq_tol(result, 4294967296.0f,
                         10.0f);  // Допуск шире из-за специфики float
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_bits2_contribution) {
  s21_decimal src = {{0, 0, 1, 0}};  // Включаем только bits[2] -> это 2^64
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  // 2^64 = 18446744073709551616.0; допуск широкий из-за точности float
  ck_assert_float_eq_tol(result, 18446744073709551616.0f, 1e13f);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_max_decimal) {
  s21_decimal src = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  // Ожидаем примерно 7.9228162514e+28
  ck_assert_float_eq_tol(result, 7.922816e+28f, 1e+22f);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_zero) {
  s21_decimal src = {{0, 0, 0, 0}};
  float result = 1.0f;  // Заведомо неверное значение, чтобы проверить запись
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_float_eq_tol(result, 0.0f, 1e-7);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float_max_scale) {
  s21_decimal src = {{1, 0, 0, (28 << 16)}};  // 1 * 10^-28
  float result = 0.0;
  int status = s21_from_decimal_to_float(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_float_eq_tol(result, 1e-28f, 1e-30f);
}
END_TEST

/* ======================================================================== */
/*                         ТЕСТЫ ДЛЯ КОНВЕРТАЦИИ ИЗ FLOAT В DECIMAL         */
/* ======================================================================== */

START_TEST(test_s21_from_float_to_decimal_basic) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(1234.5f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 12345);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], (1 << 16));  // scale = 1
}
END_TEST

START_TEST(test_s21_from_float_to_decimal_negative) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(-0.12345f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 12345);
  ck_assert_int_eq(result.bits[3],
                   (5 << 16) | (1U << 31));  // scale = 5, знак -
}
END_TEST

START_TEST(test_s21_from_float_to_decimal_special) {
  s21_decimal result;
  int status_nan = s21_from_float_to_decimal(NAN, &result);
  ck_assert_int_eq(status_nan, S21_ERROR_CONVERSION);

  int status_inf = s21_from_float_to_decimal(INFINITY, &result);
  ck_assert_int_eq(status_inf, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal_overflow) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(1e30f, &result);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal_underflow) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(1e-30f, &result);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal_zero) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(0.0f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal_null) {
  int status = s21_from_float_to_decimal(1.5f, NULL);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

/* Регрессионный тест на баг из ревью: числа >= 10^7 раньше отвергались
 * с S21_ERROR_CONVERSION из-за ухода scale в минус без обработки. */
START_TEST(test_s21_from_float_to_decimal_large_integer) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(12345670.0f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 12345670);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);  // scale = 0
}
END_TEST

/* Ещё один регрессионный тест на ту же проблему, но с числом, где
 * мантисса после округления содержит незначащие нули (100000000.0 ->
 * 7-значная мантисса 1000000, затем домножение на 10^2). */
START_TEST(test_s21_from_float_to_decimal_large_integer_100m) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(100000000.0f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 100000000);
  ck_assert_int_eq(result.bits[3], 0);  // scale = 0
}
END_TEST

/* Число, требующее больше 32 бит после домножения мантиссы обратно —
 * проверяет, что перенос между bits[0]/bits[1] работает корректно. */
START_TEST(test_s21_from_float_to_decimal_large_integer_carries) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(1234567000000000.0f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[3], 0);  // scale = 0
  // Проверяем, что перенос ушёл в bits[1] (значение точно не влезает в
  // один 32-битный bits[0]).
  ck_assert_int_ne(result.bits[1], 0);
}
END_TEST

/* Тест на банковское округление: .5 округляется к ближайшему чётному.
 * 1234564.5 -> должно округлиться ВНИЗ до 1234564 (чётное), а не вверх
 * до 1234565, как дал бы обычный round(). */
START_TEST(test_s21_from_float_to_decimal_bankers_rounding_down) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(1234564.5f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 1234564);  // не 1234565
}
END_TEST

/* Симметричный случай: 1234565.5 -> должно округлиться ВВЕРХ до
 * 1234566 (чётное), поскольку 1234565 нечётное. */
START_TEST(test_s21_from_float_to_decimal_bankers_rounding_up) {
  s21_decimal result;
  int status = s21_from_float_to_decimal(1234565.5f, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result.bits[0], 1234566);
}
END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

/**
 * @brief Создает тестовый сьют для преобразователей типов.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_converters_suite(void) {
  Suite *s = suite_create("s21_converters");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_s21_from_int_to_decimal_basic);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_negative);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_zero);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_max);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_min);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_null);

  tcase_add_test(tc_core, test_s21_from_decimal_to_int_basic);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_truncate);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_truncate_negative);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_min);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_null);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_overflow_positive);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_overflow_negative);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_bits_overflow);

  tcase_add_test(tc_core, test_s21_from_decimal_to_float_basic);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_decimal_part);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_null);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_invalid_scale);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_large_mantissa);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_bits2_contribution);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_max_decimal);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_zero);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float_max_scale);

  tcase_add_test(tc_core, test_s21_from_float_to_decimal_basic);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_negative);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_special);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_overflow);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_underflow);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_zero);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_null);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_large_integer);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_large_integer_100m);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_large_integer_carries);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_bankers_rounding_down);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal_bankers_rounding_up);

  suite_add_tcase(s, tc_core);
  return s;
}