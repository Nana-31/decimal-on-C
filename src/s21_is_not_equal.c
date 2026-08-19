/**
 * @file s21_is_not_equal.c
 * @brief Реализация оператора сравнения "не равно" (!=).
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Проверяет, не равны ли два числа (!=).
 */
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
    return !s21_is_equal(value_1, value_2);
}