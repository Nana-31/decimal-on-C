/**
 * @file s21_from_decimal_to_int.c
 * @brief Реализация преобразования из s21_decimal в int.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Преобразует Decimal в целое число (int).
 *
 * @note Дробная часть отбрасывается (усечение к нулю, не округление).
 * @note Если число больше INT_MAX или меньше INT_MIN, возвращается
 * S21_ERROR_CONVERSION.
 *
 * @param src Исходное число типа Decimal.
 * @param dst Указатель на переменную для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при переполнении
 * диапазона int.
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error_code = S21_OK;

  if (dst == NULL) {
    error_code = S21_ERROR_CONVERSION;
  } else {
    s21_decimal clean_src;
    if (s21_truncate(src, &clean_src) != S21_OK) {
      error_code = S21_ERROR_CONVERSION;
    } else if (clean_src.bits[1] != 0 || clean_src.bits[2] != 0) {
      error_code = S21_ERROR_CONVERSION;
    } else {
      unsigned int raw_value = clean_src.bits[0];
      if (clean_src.bits[3] & (1U << 31)) {
        if (raw_value > 2147483648U) {
          error_code = S21_ERROR_CONVERSION;
        } else {
          *dst = (int)(0u - raw_value);
        }
      } else {
        if (raw_value > 2147483647U) {
          error_code = S21_ERROR_CONVERSION;
        } else {
          *dst = (int)raw_value;
        }
      }
    }
  }

  return error_code;
}
