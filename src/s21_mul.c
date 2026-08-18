/**
 * @file s21_mul.c
 * @brief Реализация функции умножения двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Умножает два десятичных числа.
 *
 * @param value_1 Первый множитель.
 * @param value_2 Второй множитель.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE при переполнении.
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) return S21_ERROR_CALCULATION;

  s21_decimal_zero(result);

  // 1. Определяем итоговый знак (XOR знаков операндов)
  int sign1 = s21_get_sign(&value_1);
  int sign2 = s21_get_sign(&value_2);
  int result_sign = sign1 ^ sign2;

  // 2. Складываем scale операндов
  int scale1 = s21_get_scale(&value_1);
  int scale2 = s21_get_scale(&value_2);
  int result_scale = scale1 + scale2;

  // 3. Умножаем мантиссы (96 бит * 96 бит = до 192 бит)
  uint32_t temp_result[6] = {0};

  for (int i = 0; i < 3; i++) {
    uint64_t carry = 0;
    for (int j = 0; j < 3; j++) {
      uint64_t product = (uint64_t)value_1.bits[i] * (uint64_t)value_2.bits[j];
      uint64_t sum = temp_result[i + j] + product + carry;
      temp_result[i + j] = (uint32_t)(sum & 0xFFFFFFFF);
      carry = sum >> 32;
    }
    if (carry > 0) {
      temp_result[i + 3] += (uint32_t)carry;
    }
  }

  // 4. Нормализация с банковским округлением
  int last_remainder = 0;
  int was_non_zero_remainder = 0;

  while (result_scale > 28 || temp_result[3] != 0 || temp_result[4] != 0 ||
         temp_result[5] != 0) {
    if (result_scale == 0) {
      return S21_ERROR_TOO_LARGE;
    }

    uint64_t remainder = 0;
    for (int i = 5; i >= 0; i--) {
      uint64_t current = (remainder << 32) | temp_result[i];
      temp_result[i] = (uint32_t)(current / 10);
      remainder = current % 10;
    }

    if (remainder > 0) {
      was_non_zero_remainder = 1;
    }
    last_remainder = (int)remainder;
    result_scale--;
  }

  // 5. Применяем банковское округление к полученной 96-битной мантиссе
  int round_up = 0;
  if (last_remainder > 5) {
    round_up = 1;
  } else if (last_remainder == 5) {
    if (was_non_zero_remainder) {
      round_up = 1;  // Дробная часть строго больше 0.5 (например, 0.500...001)
    } else {
      // Дробная часть ровно 0.5. Округляем к ближайшему четному.
      // Проверяем, является ли младший бит 96-битного числа нечетным.
      if (temp_result[0] & 1) {
        round_up = 1;
      }
    }
  }

  if (round_up) {
    // Прибавляем 1 к 96-битному числу с обработкой переноса
    uint64_t sum = (uint64_t)temp_result[0] + 1;
    temp_result[0] = (uint32_t)sum;
    if (sum > 0xFFFFFFFF) {
      sum = (uint64_t)temp_result[1] + 1;
      temp_result[1] = (uint32_t)sum;
      if (sum > 0xFFFFFFFF) {
        if (temp_result[2] == 0xFFFFFFFF) {
          return S21_ERROR_TOO_LARGE;  // Переполнение 96 бит после округления
        }
        temp_result[2] += 1;
      }
    }
  }

  // 6. Копируем результат в структуру
  result->bits[0] = temp_result[0];
  result->bits[1] = temp_result[1];
  result->bits[2] = temp_result[2];

  // 7. Устанавливаем scale и знак
  s21_set_scale(result, result_scale);
  s21_set_sign(result, result_sign);

  return S21_OK;
}