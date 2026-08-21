/**
 * @file s21_floor.c
 * @brief Реализация функции округления десятичного числа вниз
 *        (к ближайшему меньшему целому).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Округляет десятичное число вниз (к ближайшему меньшему целому).
 * @note Для положительных чисел результат совпадает с truncate.
 *       Для отрицательных: floor(-1.1) = -2, floor(-1.0) = -1.
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CALCULATION при ошибке.
 */
int s21_floor(s21_decimal value, s21_decimal *result) {
  int status = S21_OK;

  if (!result) {
    status = S21_ERROR_CALCULATION;
  } else {
    int original_sign = s21_get_sign(&value);

    // Шаг 1: отбрасываем дробную часть (как в s21_truncate)
    *result = value;
    status = s21_round_to_scale(result, 0, S21_ROUND_TRUNCATE);

    // Шаг 2: если число было отрицательным, проверяем наличие дробной части
    if (status == S21_OK && original_sign == 1) {
      s21_decimal diff = {{0, 0, 0, 0}};  // Защита от неопределенного поведения

      if (s21_sub(value, *result, &diff) == S21_OK) {
        if (!s21_is_zero(&diff)) {
          // Если есть дробная часть, увеличиваем модуль отрицательного числа на
          // 1
          s21_set_sign(result,
                       0);  // Сбрасываем знак для безопасной работы с модулем

          // Прибавляем 1 к мантиссе с корректным учетом переноса между блоками
          uint32_t carry = 1;
          for (int i = 0; i < 3 && carry > 0; i++) {
            uint64_t sum = (uint64_t)result->bits[i] + carry;
            result->bits[i] = (uint32_t)sum;
            carry = (uint32_t)(sum >> 32);
          }
          // Переполнение старшего блока (bits[2]) здесь математически
          // невозможно

          s21_set_scale(result, 0);
          s21_set_sign(result, 1);  // Восстанавливаем отрицательный знак
        }
      } else {
        status = S21_ERROR_CALCULATION;
      }
    }

    // Шаг 3: если результат равен нулю, знак должен быть положительным
    if (status == S21_OK && s21_is_zero(result)) {
      s21_set_sign(result, 0);
    }
  }

  return status;
}