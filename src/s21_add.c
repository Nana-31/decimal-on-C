/**
 * @file s21_add.c
 * @brief Реализация функции сложения двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Складывает два десятичных числа.
 *
 * @param value_1 Первое слагаемое.
 * @param value_2 Второе слагаемое.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE при переполнении,
 *         S21_ERROR_CALCULATION при ошибке нормализации или NULL-указателе.
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) return S21_ERROR_CALCULATION;

  s21_decimal_zero(result);

  // 1. Приводим числа к общему масштабу (scale)
  if (s21_normalize(&value_1, &value_2) != S21_OK) {
    return S21_ERROR_CALCULATION;  // Переполнение при нормализации
  }

  int sign1 = s21_get_sign(&value_1);
  int sign2 = s21_get_sign(&value_2);
  int scale = s21_get_scale(&value_1);  // После нормализации scale одинаковый

  // 2. Если знаки одинаковые, просто складываем мантиссы
  if (sign1 == sign2) {
    int status = s21_bigint_add(&value_1, &value_2, result);
    if (status != S21_OK) {
      return status;  // Пробрасываем S21_ERROR_TOO_LARGE
    }
    s21_set_sign(result, sign1);
  }
  // 3. Если знаки разные, это вычитание модулей
  else {
    int cmp = s21_bigint_cmp_abs(&value_1, &value_2);

    if (cmp == 0) {
      // Числа равны по модулю, результат - абсолютный ноль
      s21_decimal_zero(result);
      return S21_OK;
    }

    if (cmp > 0) {  // |value_1| > |value_2|
      s21_bigint_sub(&value_1, &value_2, result);
      s21_set_sign(result, sign1);
    } else {  // |value_1| < |value_2|
      s21_bigint_sub(&value_2, &value_1, result);
      s21_set_sign(result, sign2);
    }
  }

  // 4. Устанавливаем итоговый scale
  s21_set_scale(result, scale);

  return S21_OK;
}