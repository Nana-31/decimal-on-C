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
#include "../s21_decimal.h"

/* ======================================================================== */
/*                              ТЕСТЫ ДЛЯ КОНВЕРТАЦИИ ИЗ INT В DECIMAL                                     */
/* ======================================================================== */


START_TEST(test_s21_from_int_to_decimal_basic){
  s21_decimal result;
   int status = s21_from_int_to_decimal(55, &result);
   
   ck_assert_int_eq(status, S21_OK); 
   ck_assert_int_eq(result.bits[0], 55);
   ck_assert_int_eq(result.bits[1], 0);
   ck_assert_int_eq(result.bits[2], 0);
   ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_negative){
  s21_decimal result;
  int status = s21_from_int_to_decimal(-77, &result);

  ck_assert_int_eq(status, S21_OK); 
  ck_assert_int_eq(result.bits[0], 77);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], (1U<<31));
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_zero){
  s21_decimal result;
  int status = s21_from_int_to_decimal(0, &result);

  ck_assert_int_eq(status, S21_OK); 
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_max){
  s21_decimal result;
  int status = s21_from_int_to_decimal(INT_MAX, &result);

  ck_assert_int_eq(status, S21_OK); 
  ck_assert_int_eq(result.bits[0], 2147483647);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_min){
  s21_decimal result;
  int status = s21_from_int_to_decimal(INT_MIN, &result);
  ck_assert_int_eq(status, S21_OK); 
  ck_assert_int_eq(result.bits[0], 2147483648U);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], (1U<<31));
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_null) {
  int status = s21_from_int_to_decimal(10, NULL);
  ck_assert_int_eq(status, S21_ERROR_CONVERSION);
}
END_TEST

/* ======================================================================== */
/*                              ТЕСТЫ ДЛЯ КОНВЕРТАЦИИ ИЗ DECIMAL В INT                                     */
/* ======================================================================== */

START_TEST(test_s21_from_decimal_to_int_basic){
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
  src.bits[3] = (2 << 16); // Записали scale = 2 в биты 16-23

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result, 123); // Дробь должна отвалиться
}
END_TEST

START_TEST(test_s21_from_decimal_to_int_min) {
  s21_decimal src;
  src.bits[0] = 2147483648U; 
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = (1U << 31); // Знак минус

  int result = 0;
  int status = s21_from_decimal_to_int(src, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(result, INT_MIN); // Должно стать ровно -2147483648
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
  tcase_add_test(tc_core, test_s21_from_decimal_to_int_min);

  suite_add_tcase(s, tc_core);
  return s;
}