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
  // Используем массив из 6 uint32_t для хранения 192-битного результата
  uint32_t temp_result[6] = {0};

  // Базовое умножение "столбиком"
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

  // 4. Нормализуем результат, если он не помещается в 96 бит
  // ИЛИ если итоговый scale превышает максимально допустимый (28)
  while (result_scale > 28 || temp_result[3] != 0 || temp_result[4] != 0 ||
         temp_result[5] != 0) {
    if (result_scale == 0) {
      // Если scale уже 0, а число все еще не помещается в 96 бит — это
      // переполнение
      return S21_ERROR_TOO_LARGE;
    }

    // Делим 192-битное число на 10
    uint64_t remainder = 0;
    for (int i = 5; i >= 0; i--) {
      uint64_t current = (remainder << 32) | temp_result[i];
      temp_result[i] = (uint32_t)(current / 10);
      remainder = current % 10;
    }
    result_scale--;
  }

  // 5. Копируем результат в структуру (только младшие 96 бит)
  result->bits[0] = temp_result[0];
  result->bits[1] = temp_result[1];
  result->bits[2] = temp_result[2];

  // 6. Устанавливаем scale и знак
  s21_set_scale(result, result_scale);
  s21_set_sign(result, result_sign);

  return S21_OK;
}