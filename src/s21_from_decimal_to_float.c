/**
 * @file s21_from_decimal_to_float.c
 * @brief Реализация преобразования из s21_decimal в float.
 */

#include <stddef.h>

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Предвычисленные степени 10 (10^0 .. 10^28) в double.
 *
 * Используются вместо цикла последовательных делений на 10.0, чтобы
 * не накапливать ошибку округления double на каждой итерации.
 */
static const double kPowersOfTen[29] = {
    1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,
    1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
    1e20, 1e21, 1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28};

/**
 * @brief Преобразует Decimal в число с плавающей точкой (float).
 *
 * Алгоритм:
 * 1. Проверить указатель dst и корректность scale (0..28).
 * 2. Извлечь мантиссу из bits[0], bits[1], bits[2] как 96-битное целое
 *    число, накопив его в double.
 * 3. Извлечь scale из bits[3] (биты 16-23).
 * 4. Извлечь знак из bits[3] (бит 31).
 * 5. Разделить мантиссу на 10^scale (одним делением на предвычисленную
 *    степень, а не циклом) для получения дробного значения.
 * 6. Применить знак к результату.
 * 7. Привести double к float и записать результат в dst.
 *
 * @note Возможна потеря точности из-за ограничений float (23 бита мантиссы).
 *
 * @param src Исходное число типа Decimal.
 * @param dst Указатель на переменную для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при ошибке.
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int error_code = S21_OK;

  if (dst == NULL) {
    error_code = S21_ERROR_CONVERSION;
  } else {
    int scale = (src.bits[3] >> 16) & 0xFF;

    if (scale < 0 || scale > 28) {
      error_code = S21_ERROR_CONVERSION;
    } else {
      /* Собираем 96-битную мантиссу в double. Вместо каверзных констант
       * используем степени 2^32 и 2^64. */
      double mantissa = (double)(unsigned int)src.bits[0];
      mantissa += (double)(unsigned int)src.bits[1] * 4294967296.0;
      mantissa +=
          (double)(unsigned int)src.bits[2] * 4294967296.0 * 4294967296.0;

      double result = mantissa / kPowersOfTen[scale];

      if (src.bits[3] & (1U << 31)) {
        result = -result;
      }

      *dst = (float)result;
    }
  }

  return error_code;
}