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

    // Шаг 1: отбрасываем дробную часть (как truncate)
    *result = value;
    status = s21_round_to_scale(result, 0, S21_ROUND_TRUNCATE);

    // Шаг 2: если число было отрицательным, проверяем наличие дробной части
    if (status == S21_OK && original_sign == 1) {
      // Инициализируем diff нулями для защиты от неопределенного поведения
      s21_decimal diff = {{0, 0, 0, 0}};

      // Проверяем успешность вычитания
      if (s21_sub(value, *result, &diff) == S21_OK) {
        // Если разница не ноль — была дробная часть, сдвигаем вниз
        if (!s21_is_zero(&diff)) {
          // Работаем напрямую с модулем мантиссы, чтобы избежать побочных
          // эффектов
          s21_set_sign(result, 0);

          // Прибавляем 1 к мантиссе (аналогично логике в s21_round_to_scale)
          uint64_t sum = (uint64_t)result->bits[0] + 1;
          result->bits[0] = (uint32_t)sum;
          if (sum > 0xFFFFFFFF) {
            sum = (uint64_t)result->bits[1] + 1;
            result->bits[1] = (uint32_t)sum;
            if (sum > 0xFFFFFFFF) {
              // Переполнение bits[2] здесь математически невозможно,
              // так как усеченное число с дробной частью строго < макс.
              // 96-битного значения
              result->bits[2] += 1;
            }
          }

          s21_set_scale(result, 0);
          s21_set_sign(result, 1);  // Восстанавливаем отрицательный знак
        }
      } else {
        // Если s21_sub вернул ошибку, пробрасываем её дальше
        status = S21_ERROR_CALCULATION;
      }
    }

    // Шаг 3: если результат — ноль, знак должен быть положительным
    if (status == S21_OK && s21_is_zero(result)) {
      s21_set_sign(result, 0);
    }
  }

  return status;
}