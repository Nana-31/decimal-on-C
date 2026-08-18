/**
 * @file s21_add.c
 * @brief Реализация функции сложения двух десятичных чисел.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Складывает два десятичных числа.
 *
 * Алгоритм:
 * 1. Привести числа к общему масштабу (scale).
 * 2. Определить итоговый знак результата.
 * 3. Выполнить сложение или вычитание мантисс в зависимости от знаков.
 * 4. Обработать переполнение.
 *
 * @param value_1 Первое слагаемое.
 * @param value_2 Второе слагаемое.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_TOO_LARGE или S21_ERROR_TOO_SMALL при переполнении.
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result)
{
    (void)value_1;
    (void)value_2;
    (void)result;
    // TODO: Реализация сложения
    return S21_OK;
}