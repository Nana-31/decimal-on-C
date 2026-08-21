/**
 * @file test_comparison.c
 * @brief Unit-тесты для операторов сравнения s21_decimal.
 *
 * Тестируемые функции:
 * - s21_is_less
 * - s21_is_less_or_equal
 * - s21_is_greater
 * - s21_is_greater_or_equal
 * - s21_is_equal
 * - s21_is_not_equal
 *
 * @note Владелец файла: Dev 2 (Логик)
 */

#include <check.h>

#include "../s21_decimal.h"

/* ======================================================================== */
/*                              ТЕСТЫ                                       */
/* ======================================================================== */

// 1. Тестирование работы с нулями (+0, -0, нули с разным scale)
START_TEST(test_zeros) {
  s21_decimal pos_zero = {{0, 0, 0, 0}};
  s21_decimal neg_zero = {{0, 0, 0, 0x80000000}};     // Знак = 1
  s21_decimal scaled_zero = {{0, 0, 0, 0x00050000}};  // Scale = 5

  ck_assert_int_eq(s21_is_equal(pos_zero, neg_zero), 1);
  ck_assert_int_eq(s21_is_equal(pos_zero, scaled_zero), 1);
  ck_assert_int_eq(s21_is_less(pos_zero, neg_zero), 0);
  ck_assert_int_eq(s21_is_greater(pos_zero, scaled_zero), 0);
  ck_assert_int_eq(s21_is_less_or_equal(neg_zero, pos_zero), 1);
  ck_assert_int_eq(s21_is_greater_or_equal(scaled_zero, neg_zero), 1);
  ck_assert_int_eq(s21_is_not_equal(pos_zero, neg_zero), 0);
}
END_TEST

// 2. Тестирование обычных положительных и отрицательных чисел
START_TEST(test_basic_numbers) {
  s21_decimal num10 = {{10, 0, 0, 0}};
  s21_decimal num20 = {{20, 0, 0, 0}};
  s21_decimal neg10 = {{10, 0, 0, 0x80000000}};
  s21_decimal neg20 = {{20, 0, 0, 0x80000000}};

  // Положительные числа (10 < 20)
  ck_assert_int_eq(s21_is_less(num10, num20), 1);
  ck_assert_int_eq(s21_is_greater(num20, num10), 1);
  ck_assert_int_eq(s21_is_equal(num10, num10), 1);

  // Отрицательные числа (-10 > -20)
  ck_assert_int_eq(s21_is_less(neg20, neg10), 1);
  ck_assert_int_eq(s21_is_greater(neg10, neg20), 1);

  // Разные знаки (-10 < 10)
  ck_assert_int_eq(s21_is_less(neg10, num10), 1);
  ck_assert_int_eq(s21_is_greater(num10, neg10), 1);
  ck_assert_int_eq(s21_is_not_equal(neg10, num10), 1);
}
END_TEST

// 3. Тестирование выравнивания масштабов (different scales)
START_TEST(test_different_scales) {
  s21_decimal one = {{1, 0, 0, 0}};                        // 1
  s21_decimal one_point_zero = {{100, 0, 0, 0x00020000}};  // 1.00 (100 / 10^2)
  s21_decimal one_point_five = {{15, 0, 0, 0x00010000}};   // 1.5  (15 / 10^1)
  s21_decimal one_point_zero_five = {
      {105, 0, 0, 0x00020000}};  // 1.05 (105 / 10^2)

  // 1 == 1.00
  ck_assert_int_eq(s21_is_equal(one, one_point_zero), 1);
  ck_assert_int_eq(s21_is_less_or_equal(one, one_point_zero), 1);

  // 1.05 < 1.5
  ck_assert_int_eq(s21_is_less(one_point_zero_five, one_point_five), 1);
  ck_assert_int_eq(s21_is_greater(one_point_five, one_point_zero_five), 1);
  ck_assert_int_eq(s21_is_not_equal(one_point_zero_five, one_point_five), 1);
}
END_TEST

// 4. Тестирование работы с большими числами (проверка ухода от переполнения
// через big_decimal)
START_TEST(test_large_numbers) {
  // Максимальное 96-битное число со scale 0
  s21_decimal max_dec = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  // Максимальное 96-битное число со scale 1 (в 10 раз меньше по значению)
  s21_decimal max_dec_scale1 = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010000}};

  ck_assert_int_eq(s21_is_greater(max_dec, max_dec_scale1), 1);
  ck_assert_int_eq(s21_is_less(max_dec_scale1, max_dec), 1);
  ck_assert_int_eq(s21_is_not_equal(max_dec, max_dec_scale1), 1);

  // Отрицательные грандиозные числа (-max_dec < -max_dec_scale1)
  s21_decimal neg_max_dec = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal neg_max_dec_scale1 = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80010000}};

  ck_assert_int_eq(s21_is_less(neg_max_dec, neg_max_dec_scale1), 1);
  ck_assert_int_eq(s21_is_greater(neg_max_dec_scale1, neg_max_dec), 1);
}
END_TEST

// 5. Тестирование составных операторов (<= и >=)
START_TEST(test_composite_operators) {
  s21_decimal a = {{50, 0, 0, 0}};
  s21_decimal b = {{50, 0, 0, 0}};
  s21_decimal c = {{100, 0, 0, 0}};

  // Равные значения
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);

  // Неравные значения
  ck_assert_int_eq(s21_is_less_or_equal(a, c), 1);
  ck_assert_int_eq(s21_is_greater_or_equal(c, a), 1);
  ck_assert_int_eq(s21_is_less_or_equal(c, a), 0);
  ck_assert_int_eq(s21_is_greater_or_equal(a, c), 0);
}
END_TEST

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

/**
 * @brief Создает тестовый сьют для операторов сравнения.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_comparison_suite(void) {
  Suite *s = suite_create("s21_comparison");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_zeros);
  tcase_add_test(tc_core, test_basic_numbers);
  tcase_add_test(tc_core, test_different_scales);
  tcase_add_test(tc_core, test_large_numbers);
  tcase_add_test(tc_core, test_composite_operators);

  suite_add_tcase(s, tc_core);
  return s;
}