/**
 * @file s21_from_decimal_to_int.c
 * @brief Реализация преобразования из s21_decimal в int.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Преобразует Decimal в целое число (int).
 *
 * Алгоритм:
 * 1. Проверить, что мантисса помещается в диапазон int (учесть знак).
 * 2. Усечь дробную часть (разделить мантиссу на 10^scale).
 * 3. Применить знак к результату.
 * 4. Записать результат в dst.
 *
 * @note Дробная часть отбрасывается (усечение к нулю, не округление).
 * @note Если число больше INT_MAX или меньше INT_MIN, возвращается S21_ERROR_CONVERSION.
 *
 * @param src Исходное число типа Decimal.
 * @param dst Указатель на переменную для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при переполнении диапазона int.
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst)
{
    (void)src;
    (void)dst;
    // TODO: Реализация конвертации
    return S21_OK;
}