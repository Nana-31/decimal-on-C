/**
 * @file s21_from_decimal_to_float.c
 * @brief Реализация преобразования из s21_decimal в float.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Преобразует Decimal в число с плавающей точкой (float).
 *
 * Алгоритм:
 * 1. Извлечь мантиссу из bits[0], bits[1], bits[2] как 96-битное целое число.
 * 2. Извлечь scale из bits[3] (биты 16-23).
 * 3. Извлечь знак из bits[3] (бит 31).
 * 4. Преобразовать мантиссу в float.
 * 5. Разделить мантиссу на 10^scale для получения дробного значения.
 * 6. Применить знак к результату.
 * 7. Записать результат в dst.
 *
 * @note Возможна потеря точности из-за ограничений float (23 бита мантиссы).
 *
 * @param src Исходное число типа Decimal.
 * @param dst Указатель на переменную для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при ошибке.
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  (void)src;
  (void)dst;
  // TODO: Реализация конвертации
  return S21_OK;
}