/**
 * @file s21_is_equal.c
 * @brief Реализация оператора сравнения "равно" (==).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Проверяет, равны ли два десятичных числа.
 *
 * Алгоритм:
 * 1. Обработать случай с нулями (+0 и -0 считаются равными).
 * 2. Если знаки разные (и числа не нули) - числа не равны.
 * 3. Если знаки одинаковые: привести к общему масштабу (scale).
 * 4. Сравнить мантиссы побитово.
 * 5. Вернуть 1, если все биты совпадают, иначе 0.
 *
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1, если value_1 == value_2, иначе 0.
 */
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int is_zero = 0;
  // TODO: Реализация сравнения
  if (s21_is_zero(&value_1) == 1 && s21_is_zero(&value_2) == 1) {
    is_zero = 1;
  }
  if (!is_zero) {
    if (s21_get_sign(&value_1) == s21_get_sign(&value_2)) {
      s21_normalize(&value_1, &value_2);
      is_zero = (
        value_1.bits[0] == value_2.bits[0] &&
        value_1.bits[1] == value_2.bits[1] &&
        value_1.bits[2] == value_2.bits[2]
      );
    }
  }
  return is_zero;
}