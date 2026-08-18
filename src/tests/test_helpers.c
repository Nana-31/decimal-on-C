/**
 * @file test_helpers.c
 * @brief Unit-тесты для внутренних вспомогательных функций s21_decimal.
 *
 * @note Владелец файла: Тимлид. Хелперы — общий фундамент команды.
 *       Любой разработчик, обнаруживший баг, обязан добавить тест и
 *       предложить фикс через PR.
 */

#include <check.h>

#include "../s21_helpers.h"

/* ======================================================================== */
/*            ТЕСТЫ ДЛЯ БАЗОВЫХ ХЕЛПЕРОВ (БИТЫ, ЗНАК, SCALE)                */
/* ======================================================================== */

START_TEST(test_s21_decimal_zero_basic) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};
  s21_decimal_zero(&value);
  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
  ck_assert_uint_eq(value.bits[3], 0);
}
END_TEST

START_TEST(test_s21_decimal_zero_already_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_decimal_zero(&value);
  ck_assert_uint_eq(value.bits[0], 0);
}
END_TEST

START_TEST(test_s21_decimal_zero_null) {
  s21_decimal_zero(NULL);  // Не должно упасть
}
END_TEST

START_TEST(test_s21_get_set_bit_basic) {
  s21_decimal value = {{0, 0, 0, 0}};

  s21_set_bit(&value, 0, 1);
  ck_assert_int_eq(s21_get_bit(&value, 0), 1);
  ck_assert_uint_eq(value.bits[0], 1);

  s21_set_bit(&value, 31, 1);
  ck_assert_int_eq(s21_get_bit(&value, 31), 1);
  ck_assert_uint_eq(value.bits[0], 0x80000001);

  s21_set_bit(&value, 32, 1);
  ck_assert_int_eq(s21_get_bit(&value, 32), 1);
  ck_assert_uint_eq(value.bits[1], 1);

  s21_set_bit(&value, 63, 1);
  ck_assert_int_eq(s21_get_bit(&value, 63), 1);

  s21_set_bit(&value, 64, 1);
  ck_assert_int_eq(s21_get_bit(&value, 64), 1);
  ck_assert_uint_eq(value.bits[2], 1);

  s21_set_bit(&value, 95, 1);
  ck_assert_int_eq(s21_get_bit(&value, 95), 1);

  s21_set_bit(&value, 0, 0);
  ck_assert_int_eq(s21_get_bit(&value, 0), 0);
}
END_TEST

START_TEST(test_s21_get_set_bit_boundaries) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};

  for (int i = 0; i < 96; i++) {
    ck_assert_int_eq(s21_get_bit(&value, i), 1);
  }

  // Биты 96-127 (служебные) тоже должны работать
  ck_assert_int_eq(s21_get_bit(&value, 96), 1);
  ck_assert_int_eq(s21_get_bit(&value, 127), 1);

  // Некорректные индексы
  ck_assert_int_eq(s21_get_bit(&value, -1), 0);
  ck_assert_int_eq(s21_get_bit(&value, 128), 0);
}
END_TEST

START_TEST(test_s21_get_set_bit_null) {
  ck_assert_int_eq(s21_get_bit(NULL, 0), 0);
  s21_set_bit(NULL, 0, 1);  // Не должно упасть
}
END_TEST

START_TEST(test_s21_get_set_sign) {
  s21_decimal value = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_get_sign(&value), 0);

  s21_set_sign(&value, 1);
  ck_assert_int_eq(s21_get_sign(&value), 1);
  ck_assert_uint_eq(value.bits[3] >> 31, 1);

  s21_set_sign(&value, 0);
  ck_assert_int_eq(s21_get_sign(&value), 0);
  ck_assert_uint_eq(value.bits[3] >> 31, 0);
}
END_TEST

START_TEST(test_s21_get_set_sign_null) {
  ck_assert_int_eq(s21_get_sign(NULL), 0);
  s21_set_sign(NULL, 1);  // Не должно упасть
}
END_TEST

START_TEST(test_s21_get_set_scale) {
  s21_decimal value = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_get_scale(&value), 0);

  s21_set_scale(&value, 10);
  ck_assert_int_eq(s21_get_scale(&value), 10);

  s21_set_scale(&value, 28);
  ck_assert_int_eq(s21_get_scale(&value), 28);

  s21_set_scale(&value, 29);
  ck_assert_int_eq(s21_get_scale(&value), 28);

  s21_set_scale(&value, -1);
  ck_assert_int_eq(s21_get_scale(&value), 28);
}
END_TEST

START_TEST(test_s21_get_set_scale_null) {
  ck_assert_int_eq(s21_get_scale(NULL), 0);
  s21_set_scale(NULL, 5);  // Не должно упасть
}
END_TEST

START_TEST(test_s21_scale_sign_independence) {
  // Проверяем, что scale и sign не затирают друг друга
  s21_decimal value = {{0, 0, 0, 0}};

  s21_set_scale(&value, 15);
  s21_set_sign(&value, 1);

  ck_assert_int_eq(s21_get_scale(&value), 15);
  ck_assert_int_eq(s21_get_sign(&value), 1);

  // Меняем scale — sign не должен измениться
  s21_set_scale(&value, 20);
  ck_assert_int_eq(s21_get_sign(&value), 1);
  ck_assert_int_eq(s21_get_scale(&value), 20);

  // Меняем sign — scale не должен измениться
  s21_set_sign(&value, 0);
  ck_assert_int_eq(s21_get_scale(&value), 20);
  ck_assert_int_eq(s21_get_sign(&value), 0);
}
END_TEST

/* ======================================================================== */
/*                    ТЕСТЫ ПРОВЕРОК И СРАВНЕНИЯ                            */
/* ======================================================================== */

START_TEST(test_s21_is_zero) {
  s21_decimal zero = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_is_zero(&zero), 1);

  s21_decimal non_zero = {{1, 0, 0, 0}};
  ck_assert_int_eq(s21_is_zero(&non_zero), 0);

  s21_decimal signed_zero = {{0, 0, 0, 0}};
  s21_set_sign(&signed_zero, 1);
  s21_set_scale(&signed_zero, 5);
  ck_assert_int_eq(s21_is_zero(&signed_zero), 1);
}
END_TEST

START_TEST(test_s21_is_zero_null) { ck_assert_int_eq(s21_is_zero(NULL), 0); }
END_TEST

START_TEST(test_s21_bigint_cmp_abs) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};

  ck_assert_int_eq(s21_bigint_cmp_abs(&a, &b), -1);
  ck_assert_int_eq(s21_bigint_cmp_abs(&b, &a), 1);
  ck_assert_int_eq(s21_bigint_cmp_abs(&a, &a), 0);

  s21_decimal c = {{100, 0, 0, 0}};
  s21_set_sign(&c, 1);
  s21_set_scale(&c, 10);
  ck_assert_int_eq(s21_bigint_cmp_abs(&a, &c), 0);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs_multi_block) {
  s21_decimal a = {{0, 1, 0, 0}};
  s21_decimal b = {{0xFFFFFFFF, 0, 0, 0}};

  ck_assert_int_eq(s21_bigint_cmp_abs(&a, &b), 1);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs_zeros) {
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_bigint_cmp_abs(&a, &b), 0);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs_null) {
  s21_decimal a = {{100, 0, 0, 0}};

  ck_assert_int_eq(s21_bigint_cmp_abs(NULL, &a), 0);
  ck_assert_int_eq(s21_bigint_cmp_abs(&a, NULL), 0);
  ck_assert_int_eq(s21_bigint_cmp_abs(NULL, NULL), 0);
}
END_TEST

/* ======================================================================== */
/*               ТЕСТЫ ДЛЯ РЕАЛИЗОВАННЫХ АРИФМЕТИЧЕСКИХ ХЕЛПЕРОВ            */
/* ======================================================================== */

START_TEST(test_s21_div_mantissa_by_10_basic) {
  s21_decimal value = {{100, 0, 0, 0}};
  int remainder = -1;

  int status = s21_div_mantissa_by_10(&value, &remainder);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 10);
  ck_assert_int_eq(remainder, 0);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_with_remainder) {
  s21_decimal value = {{123, 0, 0, 0}};
  int remainder = -1;

  int status = s21_div_mantissa_by_10(&value, &remainder);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 12);
  ck_assert_int_eq(remainder, 3);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_max_remainder) {
  // 19 / 10 = 1, остаток 9 (максимальный возможный)
  s21_decimal value = {{19, 0, 0, 0}};
  int remainder = -1;

  int status = s21_div_mantissa_by_10(&value, &remainder);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 1);
  ck_assert_int_eq(remainder, 9);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  int remainder = -1;

  int status = s21_div_mantissa_by_10(&value, &remainder);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_int_eq(remainder, 0);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_multi_block) {
  s21_decimal value = {{5, 1, 0, 0}};
  int remainder = -1;

  int status = s21_div_mantissa_by_10(&value, &remainder);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 0x1999999A);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
  ck_assert_int_eq(remainder, 1);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_large_multi_block) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  int remainder = -1;

  int status = s21_div_mantissa_by_10(&value, &remainder);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_ne(value.bits[2], 0);
  ck_assert_int_eq(remainder, 5);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_null) {
  s21_decimal value = {{100, 0, 0, 0}};
  int remainder;

  ck_assert_int_eq(s21_div_mantissa_by_10(NULL, &remainder),
                   S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_div_mantissa_by_10(&value, NULL), S21_ERROR_CALCULATION);
}
END_TEST

START_TEST(test_s21_round_to_scale_truncate) {
  s21_decimal value = {{12345, 0, 0, 0}};
  s21_set_scale(&value, 3);

  int status = s21_round_to_scale(&value, 1, S21_ROUND_TRUNCATE);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 123);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_up) {
  s21_decimal value = {{1235, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_UP);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 124);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_up_down) {
  s21_decimal value = {{1234, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_UP);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 123);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_even) {
  s21_decimal value = {{1235, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_EVEN);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 124);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_even_to_even) {
  s21_decimal value = {{1225, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_EVEN);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 122);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_negative) {
  // -12.35 с HALF_UP -> -12.4
  s21_decimal value = {{1235, 0, 0, 0}};
  s21_set_scale(&value, 2);
  s21_set_sign(&value, 1);

  int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_UP);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 124);
  ck_assert_int_eq(s21_get_sign(&value), 1);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_overflow) {
  // 9.99 -> округление до scale=0 -> 10
  s21_decimal value = {{999, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_round_to_scale(&value, 0, S21_ROUND_HALF_UP);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 10);
  ck_assert_int_eq(s21_get_scale(&value), 0);
}
END_TEST

START_TEST(test_s21_round_to_scale_no_change) {
  s21_decimal value = {{12345, 0, 0, 0}};
  s21_set_scale(&value, 3);

  int status = s21_round_to_scale(&value, 3, S21_ROUND_HALF_UP);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 12345);
  ck_assert_int_eq(s21_get_scale(&value), 3);
}
END_TEST

START_TEST(test_s21_round_to_scale_target_greater) {
  // Если target > current, ничего не должно меняться
  s21_decimal value = {{123, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_round_to_scale(&value, 5, S21_ROUND_HALF_UP);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 123);
  ck_assert_int_eq(s21_get_scale(&value), 2);
}
END_TEST

START_TEST(test_s21_round_to_scale_invalid) {
  s21_decimal value = {{100, 0, 0, 0}};

  ck_assert_int_eq(s21_round_to_scale(NULL, 0, S21_ROUND_TRUNCATE),
                   S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_round_to_scale(&value, -1, S21_ROUND_TRUNCATE),
                   S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_round_to_scale(&value, 29, S21_ROUND_TRUNCATE),
                   S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*               ТЕСТЫ ДЛЯ БАЗОВОЙ АРИФМЕТИКИ (ADD / SUB)                   */
/* ======================================================================== */

/**
 * @brief Тест базового сложения двух чисел без переноса разряда.
 */
START_TEST(test_s21_bigint_add_basic) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_add(&a, &b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 300);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_uint_eq(result.bits[3],
                    0);  // Убедимся, что служебные биты не задеты.
}
END_TEST

/**
 * @brief Тест сложения с переносом разряда из bits[0] в bits[1].
 */
START_TEST(test_s21_bigint_add_with_carry) {
  // 0xFFFFFFFF + 1 = 0x100000000 (перенос во второй блок).
  s21_decimal a = {{0xFFFFFFFF, 0, 0, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_add(&a, &b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 1);
  ck_assert_uint_eq(result.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сложения с каскадным переносом через несколько блоков.
 */
START_TEST(test_s21_bigint_add_multi_block_carry) {
  // Перенос через все блоки: (0xFFFFFFFF, 0xFFFFFFFF, 0) + 1 = (0, 0, 1).
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_add(&a, &b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 1);
}
END_TEST

/**
 * @brief Тест переполнения при сложении максимального 96-битного числа.
 */
START_TEST(test_s21_bigint_add_overflow) {
  // Максимальное 96-битное число + 1 = переполнение.
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_add(&a, &b, &result);

  ck_assert_int_eq(status, S21_ERROR_TOO_LARGE);
}
END_TEST

/**
 * @brief Тест сложения произвольного числа с нулем (нейтральный элемент).
 */
START_TEST(test_s21_bigint_add_zero) {
  s21_decimal a = {{12345, 67890, 11111, 0}};
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_add(&a, &zero, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], a.bits[0]);
  ck_assert_uint_eq(result.bits[1], a.bits[1]);
  ck_assert_uint_eq(result.bits[2], a.bits[2]);
}
END_TEST

/**
 * @brief Тест базового вычитания без займа разряда.
 */
START_TEST(test_s21_bigint_sub_basic) {
  s21_decimal a = {{300, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_sub(&a, &b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 100);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
}
END_TEST

/**
 * @brief Тест вычитания с займом разряда из bits[1] в bits[0].
 */
START_TEST(test_s21_bigint_sub_with_borrow) {
  // 0x100000000 - 1 = 0xFFFFFFFF.
  s21_decimal a = {{0, 1, 0, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;

  int status = s21_bigint_sub(&a, &b, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[1], 0);
}
END_TEST

/**
 * @brief Тест вычитания числа из самого себя (результат должен быть абсолютным
 * нулем).
 */
START_TEST(test_s21_bigint_sub_equal) {
  // Вычитание числа из самого себя должно давать абсолютный ноль.
  s21_decimal a = {{12345, 67890, 11111, 0}};
  s21_decimal result;

  int status = s21_bigint_sub(&a, &a, &result);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
}
END_TEST

/**
 * @brief Тест обработки NULL-указателей для функций сложения и вычитания.
 */
START_TEST(test_s21_bigint_null_pointers) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_decimal result;

  // Проверяем все комбинации NULL для s21_bigint_add.
  ck_assert_int_eq(s21_bigint_add(NULL, &b, &result), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_bigint_add(&a, NULL, &result), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_bigint_add(&a, &b, NULL), S21_ERROR_CALCULATION);

  // Проверяем все комбинации NULL для s21_bigint_sub.
  ck_assert_int_eq(s21_bigint_sub(NULL, &b, &result), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_bigint_sub(&a, NULL, &result), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_bigint_sub(&a, &b, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*                 ТЕСТЫ ДЛЯ s21_mul_mantissa_by_10                         */
/* ======================================================================== */

/**
 * @brief Тест базового умножения мантиссы на 10.
 */
START_TEST(test_s21_mul_mantissa_by_10_basic) {
  s21_decimal value = {{123, 0, 0, 0}};
  s21_set_scale(&value, 2);

  int status = s21_mul_mantissa_by_10(&value);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 1230);
  ck_assert_int_eq(s21_get_scale(&value), 3);  // Scale увеличился на 1.
}
END_TEST

/**
 * @brief Тест умножения нуля на 10.
 */
START_TEST(test_s21_mul_mantissa_by_10_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_set_scale(&value, 5);

  int status = s21_mul_mantissa_by_10(&value);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_int_eq(s21_get_scale(&value), 6);
}
END_TEST

/**
 * @brief Тест умножения с переносом между блоками.
 */
START_TEST(test_s21_mul_mantissa_by_10_with_carry) {
  // 0xFFFFFFFF * 10 = 0x9FFFFFFF6 (перенос во второй блок)
  s21_decimal value = {{0xFFFFFFFF, 0, 0, 0}};
  s21_set_scale(&value, 0);

  int status = s21_mul_mantissa_by_10(&value);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(value.bits[0], 0xFFFFFFF6);
  ck_assert_uint_eq(value.bits[1], 0x9);
  ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

/**
 * @brief Тест переполнения при умножении максимального числа.
 */
START_TEST(test_s21_mul_mantissa_by_10_overflow) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};

  int status = s21_mul_mantissa_by_10(&value);

  ck_assert_int_eq(status, S21_ERROR_TOO_LARGE);
}
END_TEST

/**
 * @brief Тест ошибки при максимальном scale (28).
 */
START_TEST(test_s21_mul_mantissa_by_10_max_scale) {
  s21_decimal value = {{100, 0, 0, 0}};
  s21_set_scale(&value, 28);

  int status = s21_mul_mantissa_by_10(&value);

  ck_assert_int_eq(status, S21_ERROR_TOO_LARGE);
  // Мантисса не должна измениться.
  ck_assert_uint_eq(value.bits[0], 100);
  ck_assert_int_eq(s21_get_scale(&value), 28);
}
END_TEST

/**
 * @brief Тест обработки NULL-указателя.
 */
START_TEST(test_s21_mul_mantissa_by_10_null) {
  ck_assert_int_eq(s21_mul_mantissa_by_10(NULL), S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*                      ТЕСТЫ ДЛЯ s21_normalize                             */
/* ======================================================================== */

/**
 * @brief Тест нормализации чисел с одинаковым scale.
 */
START_TEST(test_s21_normalize_equal_scales) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{200, 0, 0, 0}};
  s21_set_scale(&a, 2);
  s21_set_scale(&b, 2);

  int status = s21_normalize(&a, &b);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(s21_get_scale(&a), 2);
  ck_assert_int_eq(s21_get_scale(&b), 2);
  ck_assert_uint_eq(a.bits[0], 100);
  ck_assert_uint_eq(b.bits[0], 200);
}
END_TEST

/**
 * @brief Тест нормализации чисел с разными scale.
 */
START_TEST(test_s21_normalize_different_scales) {
  s21_decimal a = {{15, 0, 0, 0}};   // 1.5 (scale=1)
  s21_decimal b = {{225, 0, 0, 0}};  // 2.25 (scale=2)
  s21_set_scale(&a, 1);
  s21_set_scale(&b, 2);

  int status = s21_normalize(&a, &b);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(a.bits[0], 150);  // 1.50
  ck_assert_int_eq(s21_get_scale(&a), 2);
  ck_assert_uint_eq(b.bits[0], 225);  // 2.25
  ck_assert_int_eq(s21_get_scale(&b), 2);
}
END_TEST

/**
 * @brief Тест нормализации с нулем.
 */
START_TEST(test_s21_normalize_with_zero) {
  s21_decimal a = {{0, 0, 0, 0}};    // 0 (scale=0)
  s21_decimal b = {{123, 0, 0, 0}};  // 1.23 (scale=2)
  s21_set_scale(&b, 2);

  int status = s21_normalize(&a, &b);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(a.bits[0], 0);
  ck_assert_int_eq(s21_get_scale(&a), 2);
  ck_assert_uint_eq(b.bits[0], 123);
  ck_assert_int_eq(s21_get_scale(&b), 2);
}
END_TEST

/**
 * @brief Тест ошибки при переполнении во время нормализации.
 */
START_TEST(test_s21_normalize_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // Max (scale=0)
  s21_decimal b = {{1, 0, 0, 0}};                             // 0.01 (scale=2)
  s21_set_scale(&b, 2);

  int status = s21_normalize(&a, &b);

  ck_assert_int_eq(status, S21_ERROR_CALCULATION);
}
END_TEST

/**
 * @brief Тест ошибки при достижении scale=28 во время нормализации.
 */
START_TEST(test_s21_normalize_max_scale) {
  s21_decimal a = {{100, 0, 0, 0}};  // scale=0
  s21_decimal b = {{200, 0, 0, 0}};  // scale=28
  s21_set_scale(&b, 28);

  int status = s21_normalize(&a, &b);

  ck_assert_int_eq(status, S21_ERROR_CALCULATION);
}
END_TEST

/**
 * @brief Тест обработки NULL-указателей.
 */
START_TEST(test_s21_normalize_null) {
  s21_decimal a = {{100, 0, 0, 0}};

  ck_assert_int_eq(s21_normalize(NULL, &a), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_normalize(&a, NULL), S21_ERROR_CALCULATION);
  ck_assert_int_eq(s21_normalize(NULL, NULL), S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*               ТЕСТЫ ДЛЯ БИТОВЫХ СДВИГОВ (SHIFT LEFT/RIGHT)               */
/* ======================================================================== */

/**
 * @brief Тест базового сдвига влево на 1 бит.
 */
START_TEST(test_s21_bigint_shift_left_basic) {
  s21_decimal value = {{1, 0, 0, 0}};

  s21_bigint_shift_left(&value, 1);

  ck_assert_uint_eq(value.bits[0], 2);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево на 32 бита (переход между блоками).
 */
START_TEST(test_s21_bigint_shift_left_32) {
  s21_decimal value = {{0xFFFFFFFF, 0, 0, 0}};

  s21_bigint_shift_left(&value, 32);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0xFFFFFFFF);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево на 64 бита.
 */
START_TEST(test_s21_bigint_shift_left_64) {
  s21_decimal value = {{1, 0, 0, 0}};

  s21_bigint_shift_left(&value, 64);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 1);
}
END_TEST

/**
 * @brief Тест сдвига влево с потерей битов (переполнение 96 бит).
 */
START_TEST(test_s21_bigint_shift_left_overflow) {
  s21_decimal value = {{0x80000000, 0, 0, 0}};  // Бит 31 установлен.

  s21_bigint_shift_left(&value, 1);

  // Бит 31 должен перейти в бит 0 второго блока.
  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 1);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево на 96 бит (полное обнуление).
 */
START_TEST(test_s21_bigint_shift_left_96) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};

  s21_bigint_shift_left(&value, 96);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево нуля.
 */
START_TEST(test_s21_bigint_shift_left_zero) {
  s21_decimal value = {{0, 0, 0, 0}};

  s21_bigint_shift_left(&value, 10);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево с NULL-указателем.
 */
START_TEST(test_s21_bigint_shift_left_null) {
  s21_bigint_shift_left(NULL, 10);  // Не должно упасть.
}
END_TEST

/**
 * @brief Тест базового сдвига вправо на 1 бит.
 */
START_TEST(test_s21_bigint_shift_right_basic) {
  s21_decimal value = {{2, 0, 0, 0}};

  s21_bigint_shift_right(&value, 1);

  ck_assert_uint_eq(value.bits[0], 1);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига вправо на 32 бита (переход между блоками).
 */
START_TEST(test_s21_bigint_shift_right_32) {
  s21_decimal value = {{0, 0xFFFFFFFF, 0, 0}};

  s21_bigint_shift_right(&value, 32);

  ck_assert_uint_eq(value.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига вправо на 64 бита.
 */
START_TEST(test_s21_bigint_shift_right_64) {
  s21_decimal value = {{0, 0, 1, 0}};

  s21_bigint_shift_right(&value, 64);

  ck_assert_uint_eq(value.bits[0], 1);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига вправо с заполнением старших битов нулями.
 */
START_TEST(test_s21_bigint_shift_right_zero_fill) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};

  s21_bigint_shift_right(&value, 32);

  ck_assert_uint_eq(value.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(value.bits[1], 0xFFFFFFFF);
  ck_assert_uint_eq(value.bits[2], 0);  // Старший блок обнулен.
}
END_TEST

/**
 * @brief Тест сдвига вправо на 96 бит (полное обнуление).
 */
START_TEST(test_s21_bigint_shift_right_96) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};

  s21_bigint_shift_right(&value, 96);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига вправо нуля.
 */
START_TEST(test_s21_bigint_shift_right_zero) {
  s21_decimal value = {{0, 0, 0, 0}};

  s21_bigint_shift_right(&value, 10);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига вправо с NULL-указателем.
 */
START_TEST(test_s21_bigint_shift_right_null) {
  s21_bigint_shift_right(NULL, 10);  // Не должно упасть.
}
END_TEST

/**
 * @brief Тест сдвига влево на 0 бит (ничего не должно измениться).
 */
START_TEST(test_s21_bigint_shift_left_zero_bits) {
  s21_decimal value = {{123, 456, 789, 0}};
  s21_set_sign(&value, 1);
  s21_set_scale(&value, 5);

  s21_bigint_shift_left(&value, 0);

  ck_assert_uint_eq(value.bits[0], 123);
  ck_assert_uint_eq(value.bits[1], 456);
  ck_assert_uint_eq(value.bits[2], 789);
  ck_assert_int_eq(s21_get_sign(&value), 1);  // Знак не изменился.
  ck_assert_int_eq(s21_get_scale(&value), 5);  // Scale не изменился.
}
END_TEST

/**
 * @brief Тест сдвига влево на отрицательное число бит.
 */
START_TEST(test_s21_bigint_shift_left_negative) {
  s21_decimal value = {{123, 0, 0, 0}};

  s21_bigint_shift_left(&value, -5);

  ck_assert_uint_eq(value.bits[0], 123);  // Ничего не изменилось.
}
END_TEST

/**
 * @brief Тест сдвига влево на 100 бит (> 96, полное обнуление).
 */
START_TEST(test_s21_bigint_shift_left_100) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};

  s21_bigint_shift_left(&value, 100);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево на 33 бита (32 + 1, комбинированный сдвиг).
 */
START_TEST(test_s21_bigint_shift_left_33) {
  s21_decimal value = {{1, 0, 0, 0}};

  s21_bigint_shift_left(&value, 33);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 2);  // 1 << 33 = 2 в блоке 1.
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево на 31 бит (граничный случай перед переходом блока).
 */
START_TEST(test_s21_bigint_shift_left_31) {
  s21_decimal value = {{3, 0, 0, 0}};

  s21_bigint_shift_left(&value, 31);

  ck_assert_uint_eq(
      value.bits[0],
      0x80000000);  // 3 << 31 = 0x180000000, младшие 32 бита = 0x80000000.
  ck_assert_uint_eq(value.bits[1], 1);  // Старший бит перешел в блок 1.
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига влево на 95 бит (граничный случай перед полным
 * обнулением).
 */
START_TEST(test_s21_bigint_shift_left_95) {
  s21_decimal value = {{1, 0, 0, 0}};

  s21_bigint_shift_left(&value, 95);

  ck_assert_uint_eq(value.bits[0], 0);
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0x80000000);  // 1 << 95 = бит 31 в блоке 2.
}
END_TEST

/**
 * @brief Тест сдвига влево с проверкой, что знак и scale не меняются.
 */
START_TEST(test_s21_bigint_shift_left_preserves_sign_scale) {
  s21_decimal value = {{100, 0, 0, 0}};
  s21_set_sign(&value, 1);
  s21_set_scale(&value, 10);

  s21_bigint_shift_left(&value, 5);

  ck_assert_int_eq(s21_get_sign(&value), 1);  // Знак не изменился.
  ck_assert_int_eq(s21_get_scale(&value), 10);  // Scale не изменился.
}
END_TEST

/**
 * @brief Тест сдвига вправо на 0 бит (ничего не должно измениться).
 */
START_TEST(test_s21_bigint_shift_right_zero_bits) {
  s21_decimal value = {{123, 456, 789, 0}};
  s21_set_sign(&value, 1);
  s21_set_scale(&value, 5);

  s21_bigint_shift_right(&value, 0);

  ck_assert_uint_eq(value.bits[0], 123);
  ck_assert_uint_eq(value.bits[1], 456);
  ck_assert_uint_eq(value.bits[2], 789);
  ck_assert_int_eq(s21_get_sign(&value), 1);  // Знак не изменился.
  ck_assert_int_eq(s21_get_scale(&value), 5);  // Scale не изменился.
}
END_TEST

/**
 * @brief Тест сдвига вправо на 33 бита (32 + 1, комбинированный сдвиг).
 */
START_TEST(test_s21_bigint_shift_right_33) {
  s21_decimal value = {{0, 2, 0, 0}};

  s21_bigint_shift_right(&value, 33);

  ck_assert_uint_eq(value.bits[0], 1);  // 2 >> 33 = 1 в блоке 0.
  ck_assert_uint_eq(value.bits[1], 0);
  ck_assert_uint_eq(value.bits[2], 0);
}
END_TEST

/**
 * @brief Тест сдвига вправо с потерей младших битов.
 */
START_TEST(test_s21_bigint_shift_right_loss_of_bits) {
  s21_decimal value = {{7, 0, 0, 0}};  // 7 = 0b111.

  s21_bigint_shift_right(&value, 1);

  ck_assert_uint_eq(value.bits[0], 3);  // 7 >> 1 = 3 (бит 0 потерян).
}
END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

Suite *s21_helpers_suite(void) {
  Suite *s = suite_create("s21_helpers");
  TCase *tc_basic = tcase_create("Basic");
  TCase *tc_arithmetic = tcase_create("Arithmetic");

  // Тесты базовых хелперов
  tcase_add_test(tc_basic, test_s21_decimal_zero_basic);
  tcase_add_test(tc_basic, test_s21_decimal_zero_already_zero);
  tcase_add_test(tc_basic, test_s21_decimal_zero_null);
  tcase_add_test(tc_basic, test_s21_get_set_bit_basic);
  tcase_add_test(tc_basic, test_s21_get_set_bit_boundaries);
  tcase_add_test(tc_basic, test_s21_get_set_bit_null);
  tcase_add_test(tc_basic, test_s21_get_set_sign);
  tcase_add_test(tc_basic, test_s21_get_set_sign_null);
  tcase_add_test(tc_basic, test_s21_get_set_scale);
  tcase_add_test(tc_basic, test_s21_get_set_scale_null);
  tcase_add_test(tc_basic, test_s21_scale_sign_independence);
  tcase_add_test(tc_basic, test_s21_is_zero);
  tcase_add_test(tc_basic, test_s21_is_zero_null);
  tcase_add_test(tc_basic, test_s21_bigint_cmp_abs);
  tcase_add_test(tc_basic, test_s21_bigint_cmp_abs_multi_block);
  tcase_add_test(tc_basic, test_s21_bigint_cmp_abs_zeros);
  tcase_add_test(tc_basic, test_s21_bigint_cmp_abs_null);

  // Тесты арифметических хелперов
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_basic);
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_with_remainder);
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_max_remainder);
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_zero);
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_multi_block);
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_large_multi_block);
  tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_null);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_truncate);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_up);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_up_down);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_even);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_even_to_even);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_negative);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_overflow);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_no_change);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_target_greater);
  tcase_add_test(tc_arithmetic, test_s21_round_to_scale_invalid);

  // === ТЕСТЫ ДЛЯ ADD/SUB ===
  tcase_add_test(tc_arithmetic, test_s21_bigint_add_basic);
  tcase_add_test(tc_arithmetic, test_s21_bigint_add_with_carry);
  tcase_add_test(tc_arithmetic, test_s21_bigint_add_multi_block_carry);
  tcase_add_test(tc_arithmetic, test_s21_bigint_add_overflow);
  tcase_add_test(tc_arithmetic, test_s21_bigint_add_zero);
  tcase_add_test(tc_arithmetic, test_s21_bigint_sub_basic);
  tcase_add_test(tc_arithmetic, test_s21_bigint_sub_with_borrow);
  tcase_add_test(tc_arithmetic, test_s21_bigint_sub_equal);
  tcase_add_test(tc_arithmetic, test_s21_bigint_null_pointers);

  // Тесты для mul_by_10
  tcase_add_test(tc_arithmetic, test_s21_mul_mantissa_by_10_basic);
  tcase_add_test(tc_arithmetic, test_s21_mul_mantissa_by_10_zero);
  tcase_add_test(tc_arithmetic, test_s21_mul_mantissa_by_10_with_carry);
  tcase_add_test(tc_arithmetic, test_s21_mul_mantissa_by_10_overflow);
  tcase_add_test(tc_arithmetic, test_s21_mul_mantissa_by_10_max_scale);
  tcase_add_test(tc_arithmetic, test_s21_mul_mantissa_by_10_null);

  // Тесты для normalize
  tcase_add_test(tc_arithmetic, test_s21_normalize_equal_scales);
  tcase_add_test(tc_arithmetic, test_s21_normalize_different_scales);
  tcase_add_test(tc_arithmetic, test_s21_normalize_with_zero);
  tcase_add_test(tc_arithmetic, test_s21_normalize_overflow);
  tcase_add_test(tc_arithmetic, test_s21_normalize_max_scale);
  tcase_add_test(tc_arithmetic, test_s21_normalize_null);

  // Тесты битовых сдвигов
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_basic);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_32);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_64);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_overflow);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_96);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_zero);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_null);

  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_basic);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_32);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_64);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_zero_fill);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_96);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_zero);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_null);

  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_zero_bits);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_negative);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_100);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_33);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_31);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_left_95);
  tcase_add_test(tc_arithmetic,
                 test_s21_bigint_shift_left_preserves_sign_scale);

  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_zero_bits);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_33);
  tcase_add_test(tc_arithmetic, test_s21_bigint_shift_right_loss_of_bits);

  suite_add_tcase(s, tc_basic);
  suite_add_tcase(s, tc_arithmetic);

  return s;
}