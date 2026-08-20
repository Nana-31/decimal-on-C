/**
 * @file s21_from_float_to_decimal.c
 * @brief Реализация преобразования из float в s21_decimal без использования
 * string.h
 */
#include <math.h>    // Разрешено для isnan, isinf, fabsf, round
#include <stddef.h>  // Разрешено для NULL

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Нормализует abs(val) так, чтобы мантисса содержала не более
 * 7 значащих цифр, и подбирает соответствующий scale.
 *
 * Вынесена отдельной функцией, чтобы не наращивать вложенность блоков
 * внутри s21_from_float_to_decimal (принцип структурного
 * программирования: логически целостный фрагмент — в подпрограмму).
 *
 * @param val Модуль исходного значения (val > 0).
 * @param mantissa Указатель для записи итоговой мантиссы (0..9999999).
 * @param scale Указатель для записи итогового scale.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION если scale вышел
 * за пределы 0..28.
 */
static int normalize_mantissa(double val, unsigned int *mantissa, int *scale) {
  int error_code = S21_OK;
  int local_scale = 0;

  while (val < 1000000.0 && local_scale < 28) {
    val *= 10.0;
    local_scale++;
  }
  while (val >= 10000000.0) {
    val /= 10.0;
    local_scale--;
  }

  unsigned int local_mantissa = (unsigned int)round(val);
  if (local_mantissa >= 10000000) {
    local_mantissa /= 10;
    local_scale--;
  }
  while (local_mantissa % 10 == 0 && local_scale > 0) {
    local_mantissa /= 10;
    local_scale--;
  }

  if (local_scale < 0 || local_scale > 28) {
    error_code = S21_ERROR_CONVERSION;
  } else {
    *mantissa = local_mantissa;
    *scale = local_scale;
  }

  return error_code;
}

/**
 * @brief Записывает знак и scale в bits[3] результата.
 *
 * @param sign 1, если число отрицательное, иначе 0.
 * @param scale Значение scale (0..28).
 * @param dst Указатель на результат (bits уже обнулены).
 */
static void write_sign_and_scale(int sign, int scale, s21_decimal *dst) {
  dst->bits[3] |= ((unsigned int)scale << 16);
  if (sign) {
    dst->bits[3] |= (1U << 31);
  }
}

/**
 * @brief Преобразует число с плавающей точкой (float) в формат Decimal.
 *
 * Алгоритм:
 * 1. Проверить указатель dst и специальные значения float (NaN, Inf,
 *    переполнение по модулю, слишком малое ненулевое значение).
 * 2. Обнулить результат; если src == 0, на этом закончить.
 * 3. Нормализовать |src| до мантиссы из не более 7 значащих цифр и
 *    подобрать scale (см. normalize_mantissa).
 * 4. Записать мантиссу, scale и знак в результат.
 *
 * @param src Исходное число типа float.
 * @param dst Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при переполнении,
 * потере значимости или если src является NaN/Inf.
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error_code = S21_OK;

  if (dst == NULL || isnan(src) || isinf(src)) {
    error_code = S21_ERROR_CONVERSION;
  } else if (fabsf(src) > 7.9228162514e+28f) {
    error_code = S21_ERROR_CONVERSION;  // Слишком велико для Decimal
  } else if (fabsf(src) < 1e-28f && fabsf(src) > 0.0f) {
    error_code = S21_ERROR_CONVERSION;  // Слишком мало для Decimal
  } else {
    dst->bits[0] = 0;
    dst->bits[1] = 0;
    dst->bits[2] = 0;
    dst->bits[3] = 0;

    if (src != 0.0f) {
      int sign = (src < 0.0f) ? 1 : 0;
      unsigned int mantissa = 0;
      int scale = 0;

      error_code = normalize_mantissa((double)fabsf(src), &mantissa, &scale);

      if (error_code == S21_OK) {
        dst->bits[0] = mantissa;
        write_sign_and_scale(sign, scale, dst);
      }
    }
  }

  return error_code;
}