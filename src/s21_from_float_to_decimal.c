/**
 * @file s21_from_float_to_decimal.c
 * @brief Реализация преобразования из float в s21_decimal.
 */
#include <math.h>
#include <stddef.h>
#include <stdint.h>

#include "s21_decimal.h"

#define S21_DECIMAL_MAX_VAL 7.9228162514264337593543950335e28
#define S21_DECIMAL_MIN_VAL 1e-28
#define S21_SIG_DIGITS_MULTIPLIER 1000000.0
#define S21_MAX_SIG_DIGITS 10000000

/**
 * @brief Округляет значение double до uint32_t методом банковского округления.
 */
static uint32_t bankers_round_uint32(double val) {
  uint32_t result = 0;
  double fl = floor(val);
  double frac = val - fl;

  if (frac < 0.5 - 1e-9) {
    result = (uint32_t)fl;
  } else if (frac > 0.5 + 1e-9) {
    result = (uint32_t)(fl + 1.0);
  } else {
    uint32_t int_val = (uint32_t)fl;
    result = (int_val % 2 == 0) ? int_val : int_val + 1;
  }
  return result;
}

/**
 * @brief Умножает 96-битную мантиссу на 10.
 */
static int mul_96_by_10(uint32_t *bits) {
  uint64_t carry = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t product = (uint64_t)bits[i] * 10 + carry;
    bits[i] = (uint32_t)(product & 0xFFFFFFFF);
    carry = product >> 32;
  }
  return (carry != 0);
}

/**
 * @brief Проверяет, делится ли 96-битная мантисса на 10 (не изменяя её).
 */
static int is_divisible_by_10(const uint32_t *bits) {
  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t current = (remainder << 32) | bits[i];
    remainder = current % 10;
  }
  return (remainder == 0) ? 1 : 0;
}

/**
 * @brief Делит 96-битную мантиссу на 10 и возвращает остаток.
 */
static int div_96_by_10_with_rem(uint32_t *bits) {
  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t current = (remainder << 32) | bits[i];
    bits[i] = (uint32_t)(current / 10);
    remainder = current % 10;
  }
  return (int)remainder;
}

/**
 * @brief Нормализует значение к диапазону [1.0, 10.0) и считает экспоненту.
 */
static void normalize_float_value(double *val, int *exp10) {
  if (*val >= 1.0) {
    while (*val >= 10.0) {
      *val /= 10.0;
      (*exp10)++;
    }
  } else {
    while (*val < 1.0) {
      *val *= 10.0;
      (*exp10)--;
    }
  }
}

/**
 * @brief Применяет коррекцию масштаба (умножение или банковское округление).
 */
static int apply_scale_adjustments(uint32_t *bits, int *scale) {
  int status = S21_OK;

  while (*scale < 0 && status == S21_OK) {
    if (mul_96_by_10(bits)) {
      status = S21_ERROR_CONVERSION;
    }
    (*scale)++;
  }

  while (*scale > 28 && status == S21_OK) {
    int rem = div_96_by_10_with_rem(bits);
    if (rem > 5 || (rem == 5 && (bits[0] & 1))) {
      bits[0] += 1;
      if (bits[0] == 0 && ++bits[1] == 0) {
        ++bits[2];
      }
    }
    (*scale)--;
  }
  return status;
}

/**
 * @brief Обрабатывает ненулевое значение float и записывает его в decimal.
 */
static int process_non_zero_float(float src, double val, s21_decimal *dst) {
  int status = S21_OK;
  int sign = (src < 0.0f) ? 1 : 0;
  int exp10 = 0;

  normalize_float_value(&val, &exp10);

  val *= S21_SIG_DIGITS_MULTIPLIER;
  uint32_t mantissa = bankers_round_uint32(val);

  if (mantissa >= S21_MAX_SIG_DIGITS) {
    mantissa /= 10;
    exp10++;
  }

  uint32_t bits[3] = {mantissa, 0, 0};
  int scale = 6 - exp10;

  status = apply_scale_adjustments(bits, &scale);

  // Удаление конечных нулей (теперь безопасно: сначала проверка, потом деление)
  while (scale > 0 && status == S21_OK) {
    if (is_divisible_by_10(bits)) {
      div_96_by_10_with_rem(bits);
      scale--;
    } else {
      break;
    }
  }

  if (status == S21_OK) {
    dst->bits[0] = (int)bits[0];
    dst->bits[1] = (int)bits[1];
    dst->bits[2] = (int)bits[2];
    dst->bits[3] = ((uint32_t)scale << 16);
    if (sign) {
      dst->bits[3] |= 0x80000000;
    }
  }
  return status;
}

/**
 * @brief Преобразует число с плавающей точкой (float) в формат Decimal.
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int status = S21_OK;

  // Исключение из правила: предварительная проверка аргументов
  if (dst == NULL) {
    return S21_ERROR_CONVERSION;
  }

  dst->bits[0] = 0;
  dst->bits[1] = 0;
  dst->bits[2] = 0;
  dst->bits[3] = 0;

  if (isnan(src) || isinf(src)) {
    status = S21_ERROR_CONVERSION;
  } else {
    double val = fabs((double)src);
    if (val > S21_DECIMAL_MAX_VAL || (val < S21_DECIMAL_MIN_VAL && val > 0.0)) {
      status = S21_ERROR_CONVERSION;
    } else if (val == 0.0) {
      status = S21_OK;
    } else {
      status = process_non_zero_float(src, val, dst);
    }
  }

  return status;
}