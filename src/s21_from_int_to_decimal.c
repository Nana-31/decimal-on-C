/**
 * @file s21_from_int_to_decimal.c
 * @brief Реализация преобразования из int в s21_decimal.
 */

#include "s21_decimal.h"
#include "s21_helpers.h"

/**
 * @brief Преобразует целое число (int) в формат Decimal.
 *
 * Алгоритм:
 * 1. Определить знак числа (отрицательное или положительное).
 * 2. Получить абсолютное значение числа.
 * 3. Записать абсолютное значение в мантиссу (bits[0], bits[1], bits[2]).
 * 4. Установить scale = 0 (целое число без дробной части).
 * 5. Установить знак в bits[3].
 *
 * @param src Исходное целое число.
 * @param dst Указатель на структуру для записи результата.
 * @return int S21_OK при успехе, S21_ERROR_CONVERSION при ошибке.
 */
 
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int error_code = S21_OK;

  if (dst ==NULL){
    error_code=S21_ERROR_CONVERSION;
  }else{
    dst->bits[0]=0;
    dst->bits[1]=0;
    dst->bits[2]=0;
    dst->bits[3]=0;

    if (src<0){
      dst->bits[3] |= (1U<<31);
      dst->bits[0]= ~(unsigned int)src+1u;
    }else{
      dst->bits[0]= (unsigned int)src;
    }
    
  }

  return error_code;
}