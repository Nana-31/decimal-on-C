/**
 * @file s21_sub.c
 * @brief Реализация функции вычитания двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Вычитает второе десятичное число из первого.
 *
 * @param value_1 Уменьшаемое.
 * @param value_2 Вычитаемое.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE при переполнении,
 *         S21_ERROR_CALCULATION при ошибке.
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) return S21_ERROR_CALCULATION;

  // Вычитание - это сложение с инвертированным знаком второго числа
  s21_decimal neg_value_2 = value_2;
  s21_set_sign(&neg_value_2, !s21_get_sign(&value_2));

  return s21_add(value_1, neg_value_2, result);
}