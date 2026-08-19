/**
 * @file s21_is_greater_or_equal.c
 * @brief Реализация оператора сравнения "больше или равно" (>=).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Проверяет, больше ли первое число или равно второму (>=).
 */
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
    return s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2);
}