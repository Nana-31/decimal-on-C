/**
 * @file s21_from_int_to_decimal.c
 * @brief Реализация преобразования из int в s21_decimal.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Преобразует целое число (int) в формат Decimal.
 *
 * Алгоритм:
 * 1. Определить знак числа (отрицательное или положительное).
 * 2. Получить абсолютное значение числа.
 * 3. Записать абсолютное значение в мантиссу (bits[0], bits[1], bits[2]).
 * 4. Установить scale = 0 (целое число без дробной части).
 * 5. Установить знак в bits[3].
 *
 * @param src Исходное целое число.
 * @param dst Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при ошибке.
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  (void)src;
  (void)dst;
  // TODO: Реализация конвертации
  return S21_OK;
}