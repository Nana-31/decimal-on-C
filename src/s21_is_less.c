/**
 * @file s21_is_less.c
 * @brief Реализация оператора сравнения "меньше" (<).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Проверяет, меньше ли первое число второго.
 *
 * Алгоритм:
 * 1. Обработать случай с нулями (+0 и -0 считаются равными).
 * 2. Если знаки разные: отрицательное всегда меньше положительного.
 * 3. Если знаки одинаковые: сравнить модули с учетом масштаба (scale).
 * 4. Для положительных чисел: |a| < |b| означает a < b.
 * 5. Для отрицательных чисел: |a| > |b| означает a < b (инверсия).
 *
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1, если value_1 < value_2, иначе 0.
 */
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int is_less = 0;

  // Сбрасываем знак у нулей для упрощения логики
  if (s21_is_zero(&value_1)) s21_set_sign(&value_1, 0);
  if (s21_is_zero(&value_2)) s21_set_sign(&value_2, 0);

  int sign_1 = s21_get_sign(&value_1);
  int sign_2 = s21_get_sign(&value_2);

  // Если знаки разные, все очевидно
  if (sign_1 != sign_2) {
    is_less = (sign_1 > sign_2);
  } else {
    // Конвертируем в Big Decimal
    s21_big_decimal big_1 = {0};
    s21_big_decimal big_2 = {0};

    s21_to_big_decimal(value_1, &big_1);
    s21_to_big_decimal(value_2, &big_2);

    // Безопасно нормализуем
    s21_normalize_big(&big_1, &big_2);

    // Сравниваем огромные мантиссы
    int cmp = s21_cmp_big_mantissa(&big_1, &big_2);

    // Обрабатываем результат с учетом знака
    if (sign_1 == 0) {
      is_less = (cmp == -1);  // Для положительных: |a| < |b| -> a < b
    } else {
      is_less = (cmp == 1);  // Для отрицательных: |a| > |b| -> a < b
    }
  }

  return is_less;
}