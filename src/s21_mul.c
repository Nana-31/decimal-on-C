/**
 * @file s21_mul.c
 * @brief Реализация функции умножения двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Умножает две 96-битные мантиссы, записывая результат в 192-битный
 * массив.
 *
 * @param v1 Указатель на первый множитель.
 * @param v2 Указатель на второй множитель.
 * @param res Указатель на массив из 6 элементов uint32_t для результата.
 */
static void s21_mul_mantissas_192(const s21_decimal *v1, const s21_decimal *v2,
                                  uint32_t *res) {
  for (int i = 0; i < 3; i++) {
    uint64_t carry = 0;
    for (int j = 0; j < 3; j++) {
      uint64_t product = (uint64_t)v1->bits[i] * (uint64_t)v2->bits[j];
      uint64_t sum = res[i + j] + product + carry;
      res[i + j] = (uint32_t)(sum & 0xFFFFFFFF);
      carry = sum >> 32;
    }
    if (carry > 0) {
      res[i + 3] += (uint32_t)carry;
    }
  }
}

/**
 * @brief Нормализует 192-битное число и применяет банковское округление.
 *
 * @param res Указатель на 192-битный массив для нормализации и округления.
 * @param scale Указатель на переменную масштаба для её уменьшения при делении.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE при переполнении.
 */
static int s21_normalize_and_round_192(uint32_t *res, int *scale) {
  int status = S21_OK;
  int last_rem = 0;
  int was_non_zero = 0;

  while (*scale > 28 || res[3] != 0 || res[4] != 0 || res[5] != 0) {
    if (*scale == 0) {
      status = S21_ERROR_TOO_LARGE;
      break;
    }

    uint64_t rem = 0;
    for (int i = 5; i >= 0; i--) {
      uint64_t current = (rem << 32) | res[i];
      res[i] = (uint32_t)(current / 10);
      rem = current % 10;
    }

    if (rem > 0) was_non_zero = 1;
    last_rem = (int)rem;
    (*scale)--;
  }

  if (status == S21_OK) {
    int round_up = 0;
    if (last_rem > 5) {
      round_up = 1;
    } else if (last_rem == 5) {
      if (was_non_zero || (res[0] & 1)) {
        round_up = 1;
      }
    }

    if (round_up) {
      uint64_t sum0 = (uint64_t)res[0] + 1;
      res[0] = (uint32_t)sum0;

      if (sum0 <= 0xFFFFFFFF) {
        // Переноса нет, округление завершено
      } else if (res[1] != 0xFFFFFFFF) {
        res[1] += 1;
      } else if (res[2] != 0xFFFFFFFF) {
        res[1] = 0;
        res[2] += 1;
      } else {
        status = S21_ERROR_TOO_LARGE;
      }
    }
  }
  return status;
}

/**
 * @brief Умножает два десятичных числа.
 *
 * @param value_1 Первый множитель.
 * @param value_2 Второй множитель.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE при переполнении,
 *         S21_ERROR_CALCULATION при ошибке.
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = S21_OK;

  if (!result) {
    return S21_ERROR_CALCULATION;
  }

  s21_decimal_zero(result);

  int result_sign = s21_get_sign(&value_1) ^ s21_get_sign(&value_2);
  int result_scale = s21_get_scale(&value_1) + s21_get_scale(&value_2);

  uint32_t temp_result[6] = {0};
  s21_mul_mantissas_192(&value_1, &value_2, temp_result);

  status = s21_normalize_and_round_192(temp_result, &result_scale);

  if (status == S21_OK) {
    result->bits[0] = temp_result[0];
    result->bits[1] = temp_result[1];
    result->bits[2] = temp_result[2];
    s21_set_scale(result, result_scale);
    s21_set_sign(result, result_sign);
  }

  return status;
}