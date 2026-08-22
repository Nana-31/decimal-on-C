/**
 * @file s21_from_float_to_decimal.c
 * @brief Реализация преобразования из float в s21_decimal без использования
 * string.h
 */
#include <math.h>  // Разрешено для isnan, isinf, fabsf, floor, fmod
#include <stddef.h>  // Разрешено для NULL

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Округляет double до ближайшего целого банковским методом
 * (округление половинок к чётному), как того требует спецификация Decimal.
 *
 * @param val Значение для округления (val >= 0).
 * @return double Округлённое значение.
 */
static double bankers_round(double val) {
  double floor_val = floor(val);
  double fractional = val - floor_val;
  double result;

  if (fractional < 0.5) {
    result = floor_val;
  } else if (fractional > 0.5) {
    result = floor_val + 1.0;
  } else if (fmod(floor_val, 2.0) == 0.0) {
    result = floor_val;  // Уже чётное — оставляем как есть
  } else {
    result = floor_val + 1.0;  // Нечётное — округляем вверх до чётного
  }

  return result;
}

/**
 * @brief Умножает 96-битное число (bits[0..2]) на 10 с переносом между
 * словами.
 *
 * Вынесена отдельной функцией, чтобы использовать её в цикле при переносе
 * мантиссы обратно в диапазон scale = 0 для больших чисел (принцип
 * структурного программирования: повторяющийся фрагмент — в подпрограмму).
 *
 * @param dst Decimal, чьи bits[0..2] нужно умножить на 10 (bits[3] не
 * трогается).
 */
static void multiply_mantissa_by_ten(s21_decimal *dst) {
  unsigned long long carry = 0;
  int i = 0;

  for (i = 0; i < 3; i++) {
    unsigned long long product =
        (unsigned long long)(unsigned int)dst->bits[i] * 10ULL + carry;
    dst->bits[i] = (int)(unsigned int)(product & 0xFFFFFFFFULL);
    carry = product >> 32;
  }
}

/**
 * @brief Нормализует abs(val) так, чтобы мантисса содержала не более
 * 7 значащих цифр, и подбирает соответствующий scale.
 *
 * @note scale здесь может получиться отрицательным (для чисел >= 10^7) —
 * это не ошибка, а сигнал вызывающей функции, что мантиссу нужно домножить
 * на 10^|scale| и использовать итоговый scale = 0 (см.
 * s21_from_float_to_decimal). Ошибкой считается только случай, когда
 * scale вышел за пределы 28 сверху (число слишком маленькое).
 *
 * @param val Модуль исходного значения (val > 0).
 * @param mantissa Указатель для записи итоговой мантиссы (0..9999999).
 * @param scale Указатель для записи итогового scale (может быть < 0).
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION если число
 * слишком маленькое (scale > 28).
 */
static int normalize_mantissa(double val, unsigned int *mantissa, int *scale) {
  int error_code = S21_OK;
  int digit_scale = 0;

  while (val < 1000000.0) {
    val *= 10.0;
    digit_scale++;
  }
  while (val >= 10000000.0) {
    val /= 10.0;
    digit_scale--;
  }

  unsigned int local_mantissa = (unsigned int)bankers_round(val);
  if (local_mantissa >= 10000000) {
    local_mantissa /= 10;
    digit_scale--;
  }
  while (local_mantissa != 0 && local_mantissa % 10 == 0 && digit_scale > 0) {
    local_mantissa /= 10;
    digit_scale--;
  }

  if (digit_scale > 28) {
    error_code = S21_ERROR_CONVERSION;  // Слишком маленькое значение
  } else {
    *mantissa = local_mantissa;
    *scale = digit_scale;
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
 * @brief Приводит мантиссу и scale к финальному виду: если scale
 * отрицательный (число >= 10^7), домножает мантиссу на 10^|scale| и
 * обнуляет scale.
 *
 * Вынесена отдельно, чтобы не наращивать вложенность в основной функции.
 *
 * @param mantissa Мантисса, полученная из normalize_mantissa.
 * @param scale Scale, полученный из normalize_mantissa (может быть < 0).
 * @param dst Указатель на результат (bits[0..2] будут перезаписаны).
 * @return int Итоговый scale (всегда 0..28).
 */
static int finalize_mantissa_and_scale(unsigned int mantissa, int scale,
                                       s21_decimal *dst) {
  dst->bits[0] = (int)mantissa;

  while (scale < 0) {
    multiply_mantissa_by_ten(dst);
    scale++;
  }

  return scale;
}

/**
 * @brief Преобразует число с плавающей точкой (float) в формат Decimal.
 *
 * Алгоритм:
 * 1. Проверить указатель dst и специальные значения float (NaN, Inf,
 *    переполнение по модулю, слишком малое ненулевое значение).
 * 2. Обнулить результат; если src == 0, на этом закончить.
 * 3. Нормализовать |src| до мантиссы из не более 7 значащих цифр и
 *    подобрать scale (см. normalize_mantissa), используя банковское
 *    округление.
 * 4. Если scale получился отрицательным (число >= 10^7), домножить
 *    мантиссу на 10^|scale| и привести scale к 0 (см.
 *    finalize_mantissa_and_scale).
 * 5. Записать мантиссу, scale и знак в результат.
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
        scale = finalize_mantissa_and_scale(mantissa, scale, dst);
        write_sign_and_scale(sign, scale, dst);
      }
    }
  }

  return error_code;
}