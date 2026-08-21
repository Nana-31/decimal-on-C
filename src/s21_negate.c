/**
 * @file s21_negate.c
 * @brief Реализация функции смены знака десятичного числа.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Инвертирует знак десятичного числа.
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CALCULATION при ошибке.
 */
int s21_negate(s21_decimal value, s21_decimal *result) {
  int status = S21_OK;

  if (!result) {
    status = S21_ERROR_CALCULATION;
  } else {
    *result = value;

    if (s21_is_zero(&value)) {
      s21_set_sign(result, 0);  // Ноль всегда положительный
    } else {
      s21_set_sign(result, !s21_get_sign(&value));  // Инвертируем знак
    }
  }

  return status;
}