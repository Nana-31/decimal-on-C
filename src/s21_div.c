/**
 * @file s21_div.c
 * @brief Реализация функции деления двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Делит первое десятичное число на второе.
 *
 * @param value_1 Делимое.
 * @param value_2 Делитель.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_DIV_BY_ZERO при делении на ноль.
 */
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) return S21_ERROR_CALCULATION;
  s21_decimal_zero(result);

  if (s21_is_zero(&value_2)) return S21_ERROR_DIV_BY_ZERO;
  if (s21_is_zero(&value_1)) return S21_OK;

  int sign1 = s21_get_sign(&value_1);
  int sign2 = s21_get_sign(&value_2);
  int res_scale = s21_get_scale(&value_1) - s21_get_scale(&value_2);

  s21_decimal N = value_1;
  s21_decimal D = value_2;
  s21_set_sign(&N, 0);
  s21_set_sign(&D, 0);
  s21_set_scale(&N, 0);
  s21_set_scale(&D, 0);

  // 1. Подготавливаем масштаб, если делимое меньше делителя (или res_scale < 0)
  while (s21_bigint_cmp_abs(&N, &D) < 0 && res_scale < 28) {
    s21_decimal temp = N;
    if (s21_mul_mantissa_by_10(&temp) == S21_OK) {
      N = temp;
      res_scale++;
    } else {
      break;  // Переполнение, дальше умножать нельзя
    }
  }

  s21_decimal quotient = {{0, 0, 0, 0}};
  s21_decimal current_N = N;
  s21_decimal current_D = D;
  int shift = 0;

  // 2. Целочисленное деление (выравнивание делителя по старшему биту)
  while (s21_bigint_cmp_abs(&current_D, &current_N) <= 0) {
    if (s21_get_bit(&current_D, 95))
      break;  // Защита от переполнения при сдвиге
    s21_bigint_shift_left(&current_D, 1);
    shift++;
  }

  // Если после цикла делитель стал больше делимого, откатываем последний сдвиг
  if (s21_bigint_cmp_abs(&current_D, &current_N) > 0) {
    s21_bigint_shift_right(&current_D, 1);
    shift--;
  }

  // Бинарное деление "в столбик"
  for (int i = shift; i >= 0; i--) {
    if (s21_bigint_cmp_abs(&current_N, &current_D) >= 0) {
      s21_bigint_sub(&current_N, &current_D, &current_N);
      s21_set_bit(&quotient, i, 1);
    }
    s21_bigint_shift_right(&current_D, 1);
  }

  // 3. Дробная часть (если остался остаток и есть место в scale)
  while (!s21_is_zero(&current_N) && res_scale < 28) {
    if (s21_mul_mantissa_by_10(&current_N) != S21_OK) break;
    res_scale++;

    // Сдвигаем уже найденное частное влево (умножаем на 10)
    s21_mul_mantissa_by_10(&quotient);

    // Находим следующую цифру частного (она будет от 1 до 9)
    int digit = 0;
    while (s21_bigint_cmp_abs(&current_N, &D) >= 0) {
      s21_bigint_sub(&current_N, &D, &current_N);
      digit++;
    }

    // Добавляем цифру к частному
    s21_decimal digit_dec = {{(uint32_t)digit, 0, 0, 0}};
    s21_bigint_add(&quotient, &digit_dec, &quotient);
  }

  // 4. Формируем итоговый результат
  result->bits[0] = quotient.bits[0];
  result->bits[1] = quotient.bits[1];
  result->bits[2] = quotient.bits[2];

  s21_set_scale(result, res_scale);
  s21_set_sign(result, sign1 ^ sign2);

  return S21_OK;
}