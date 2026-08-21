/**
 * @file s21_truncate.c
 * @brief Реализация функции отбрасывания дробной части десятичного числа.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Отбрасывает дробную часть десятичного числа.
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CALCULATION при ошибке.
 */
int s21_truncate(s21_decimal value, s21_decimal *result) {
  int status = S21_OK;

  if (!result) {
    status = S21_ERROR_CALCULATION;
  } else {
    *result = value;
    status = s21_round_to_scale(result, 0, S21_ROUND_TRUNCATE);

    // Если результат равен нулю, знак должен быть положительным
    if (status == S21_OK && s21_is_zero(result)) {
      s21_set_sign(result, 0);
    }
  }

  return status;
}