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
 * @return int S21_OK при успехе, S21_ERROR_DIV_BY_ZERO при делении на ноль,
 *         S21_ERROR_TOO_LARGE при переполнении.
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

    // Находим следующую цифру частного (она будет от 0 до 9)
    int digit = 0;
    while (s21_bigint_cmp_abs(&current_N, &D) >= 0) {
      s21_bigint_sub(&current_N, &D, &current_N);
      digit++;
    }

    // Добавляем цифру к частному
    s21_decimal digit_dec = {{(uint32_t)digit, 0, 0, 0}};
    s21_bigint_add(&quotient, &digit_dec, &quotient);
  }

  // 4. Банковское округление, если после достижения scale=28 остался остаток
  if (!s21_is_zero(&current_N)) {
    // Нам нужно узнать, больше ли остаток половины делителя, равен или меньше.
    // Сравниваем (current_N * 2) с D. Делаем это вручную, чтобы избежать
    // ложного S21_ERROR_TOO_LARGE от s21_bigint_add при переполнении 96-го
    // бита.

    int cmp = 0;
    if (current_N.bits[2] >> 31) {
      // Если при сдвиге влево есть перенос за пределы 96 бит,
      // значит current_N * 2 > 2^96, что заведомо больше любого 96-битного D.
      cmp = 1;
    } else {
      s21_decimal current_N_x2;
      current_N_x2.bits[0] = current_N.bits[0] << 1;
      current_N_x2.bits[1] =
          (current_N.bits[1] << 1) | (current_N.bits[0] >> 31);
      current_N_x2.bits[2] =
          (current_N.bits[2] << 1) | (current_N.bits[1] >> 31);
      current_N_x2.bits[3] = 0;
      cmp = s21_bigint_cmp_abs(&current_N_x2, &D);
    }

    int round_up = 0;
    if (cmp > 0) {
      // Остаток строго больше половины делителя (> 0.5)
      round_up = 1;
    } else if (cmp == 0) {
      // Остаток ровно половина делителя (== 0.5)
      // Проверяем четность последней цифры частного для банковского округления
      uint32_t last_digit = quotient.bits[0] % 10;
      if (last_digit % 2 != 0) {
        round_up = 1;  // Округляем к ближайшему четному (увеличиваем нечетное)
      }
    }
    // Если cmp < 0, round_up остается 0 (< 0.5, округляем вниз)

    if (round_up) {
      s21_decimal one = {{1, 0, 0, 0}};
      // Если при округлении произошло переполнение 96 бит, это ошибка
      if (s21_bigint_add(&quotient, &one, &quotient) != S21_OK) {
        return S21_ERROR_TOO_LARGE;
      }
    }
  }

  // 5. Формируем итоговый результат
  result->bits[0] = quotient.bits[0];
  result->bits[1] = quotient.bits[1];
  result->bits[2] = quotient.bits[2];

  s21_set_scale(result, res_scale);
  s21_set_sign(result, sign1 ^ sign2);

  return S21_OK;
}