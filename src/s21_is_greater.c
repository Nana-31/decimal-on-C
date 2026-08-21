/**
 * @file s21_is_greater.c
 * @brief Реализация оператора сравнения "больше" (>).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Проверяет, больше ли первое число второго (>).
 * Использует зеркальную логику: A > B эквивалентно B < A.
 */
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_less(value_2, value_1);
}