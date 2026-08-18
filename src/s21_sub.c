/**
 * @file s21_sub.c
 * @brief Реализация функции вычитания двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Вычитает второе десятичное число из первого.
 *
 * Алгоритм:
 * 1. Привести числа к общему масштабу (scale).
 * 2. Определить итоговый знак результата.
 * 3. Выполнить сложение или вычитание мантисс в зависимости от знаков.
 * 4. Обработать переполнение.
 *
 * @param value_1 Уменьшаемое.
 * @param value_2 Вычитаемое.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE или S21_ERROR_TOO_SMALL при переполнении.
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result)
{
    (void)value_1;
    (void)value_2;
    (void)result;
    // TODO: Реализация вычитания
    return S21_OK;
}