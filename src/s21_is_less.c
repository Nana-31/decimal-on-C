/**
 * @file s21_is_less.c
 * @brief Реализация оператора сравнения "меньше" (<).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Проверяет, меньше ли первое число второго.
 *
 * Алгоритм:
 * 1. Обработать случай с нулями (+0 и -0 считаются равными).
 * 2. Если знаки разные: отрицательное всегда меньше положительного.
 * 3. Если знаки одинаковые: сравнить модули с учетом масштаба (scale).
 * 4. Для положительных чисел: |a| < |b| означает a < b.
 * 5. Для отрицательных чисел: |a| > |b| означает a < b (инверсия).
 *
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1, если value_1 < value_2, иначе 0.
 */
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int is_less = 0;
  int is_zero = 0;
  // TODO: Реализация сравнения
  if (s21_is_zero(&value_1) == 1 && s21_is_zero(&value_2) == 1) {
    is_zero = 1;
  }

  if (!is_zero) {
    if (s21_get_sign(&value_1) > s21_get_sign(&value_2)) {
      is_less = 1;
    }

  if (!is_zero && s21_get_sign(&value_1) == s21_get_sign(&value_2)) {
    s21_normalize(&value_1, &value_2);

    if (!s21_get_sign(&value_1)) {
      if (s21_bigint_cmp_abs(&value_1, &value_2) == -1) {
        is_less = 1;
      }
    }

    else {
      if (s21_bigint_cmp_abs(&value_1, &value_2) == 1) {
        is_less = 1;
      }
    }
  }
}
  return is_less;
}