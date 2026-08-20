#ifndef S21_DECIMAL_H_
#define S21_DECIMAL_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Коды возврата функций библиотеки s21_decimal.
 *
 * @note Значение 1 используется как универсальный код ошибки для разных
 *       категорий функций согласно требованиям спецификации. Разные имена
 *       макросов оставлены для улучшения читаемости кода (self-documenting).
 *
 * Арифметические операторы:
 *   0 — OK;
 *   1 — число слишком велико или равно бесконечности;
 *   2 — число слишком мало или равно отрицательной бесконечности;
 *   3 — деление на 0.
 *
 * Операторы сравнения:
 *   0 — FALSE;
 *   1 — TRUE.
 *
 * Операторы преобразования и прочие функции:
 *   0 — OK;
 *   1 — ошибка конвертации / ошибка вычисления.
 */
#define S21_OK 0  ///< Успешное выполнение операции
#define S21_ERROR_TOO_LARGE 1  ///< Арифметика: число слишком велико
#define S21_ERROR_TOO_SMALL 2  ///< Арифметика: число слишком мало
#define S21_ERROR_DIV_BY_ZERO 3  ///< Арифметика: деление на 0
#define S21_ERROR_CONVERSION 1  ///< Конвертеры: ошибка конвертации (значение 1)
#define S21_ERROR_CALCULATION \
  1  ///< Прочие функции: ошибка вычисления (значение 1)

/**
 * @brief Структура данных для представления десятичного числа (Decimal).
 *
 * Занимает 128 бит (16 байт) в памяти. Представляет собой число вида:
 * (-1)^sign * (mantissa / 10^scale)
 *
 * - bits[0], bits[1], bits[2]: 96-битная мантисса (целое число без знака).
 * - bits[3]: служебные биты.
 *   - Бит 31: знак числа (0 = положительное, 1 = отрицательное).
 *   - Биты 16-23: степень (scale), диапазон от 0 до 28.
 *     Указывает количество десятичных знаков после запятой.
 *   - Биты 0-15 и 24-30: зарезервированы, должны быть равны 0.
 */
typedef struct {
  uint32_t bits[4];
} s21_decimal;

/* ======================================================================== */
/*                         АРИФМЕТИЧЕСКИЕ ОПЕРАТОРЫ                         */
/* ======================================================================== */

/**
 * @brief Сложение двух десятичных чисел.
 *
 * @param value_1 Первое слагаемое.
 * @param value_2 Второе слагаемое.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0), S21_ERROR_TOO_LARGE (1) или S21_ERROR_TOO_SMALL (2).
 * @note При получении чисел, не вмещающихся в мантиссу, используется банковское
 * округление.
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Вычитание второго десятичного числа из первого.
 *
 * @param value_1 Уменьшаемое.
 * @param value_2 Вычитаемое.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0), S21_ERROR_TOO_LARGE (1) или S21_ERROR_TOO_SMALL (2).
 * @note При получении чисел, не вмещающихся в мантиссу, используется банковское
 * округление.
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Умножение двух десятичных чисел.
 *
 * @param value_1 Первый множитель.
 * @param value_2 Второй множитель.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0), S21_ERROR_TOO_LARGE (1) или S21_ERROR_TOO_SMALL (2).
 * @note При получении чисел, не вмещающихся в мантиссу, используется банковское
 * округление.
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Деление первого десятичного числа на второе.
 *
 * @param value_1 Делимое.
 * @param value_2 Делитель.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0), S21_ERROR_TOO_LARGE (1), S21_ERROR_TOO_SMALL (2) или
 * S21_ERROR_DIV_BY_ZERO (3).
 * @note При получении чисел, не вмещающихся в мантиссу, используется банковское
 * округление.
 */
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/* ======================================================================== */
/*                         ОПЕРАТОРЫ СРАВНЕНИЯ                              */
/* ======================================================================== */

/**
 * @brief Проверка: первое число меньше второго.
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1 (TRUE), если value_1 < value_2, иначе 0 (FALSE).
 */
int s21_is_less(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Проверка: первое число меньше или равно второму.
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1 (TRUE), если value_1 <= value_2, иначе 0 (FALSE).
 */
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Проверка: первое число больше второго.
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1 (TRUE), если value_1 > value_2, иначе 0 (FALSE).
 */
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Проверка: первое число больше или равно второму.
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1 (TRUE), если value_1 >= value_2, иначе 0 (FALSE).
 */
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Проверка: числа равны (включая случай +0 и -0).
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1 (TRUE), если value_1 == value_2, иначе 0 (FALSE).
 */
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Проверка: числа не равны.
 * @param value_1 Первое число.
 * @param value_2 Второе число.
 * @return int 1 (TRUE), если value_1 != value_2, иначе 0 (FALSE).
 */
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

/* ======================================================================== */
/*                         ПРЕОБРАЗОВАТЕЛИ                                  */
/* ======================================================================== */

/**
 * @brief Преобразование целого числа (int) в формат Decimal.
 *
 * @param src Исходное целое число.
 * @param dst Указатель на структуру для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CONVERSION (1).
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst);

/**
 * @brief Преобразование числа с плавающей точкой (float) в формат Decimal.
 *
 * @param src Исходное число типа float.
 * @param dst Указатель на структуру для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CONVERSION (1).
 * @note Если 0 < |x| < 1e-28, возвращается ошибка (1) и значение, равное 0.
 * @note Если |x| > 79,228,162,514,264,337,593,543,950,335 или x равен ±Inf,
 * возвращается ошибка (1).
 * @note Если значимых десятичных цифр > 7, значение округляется с
 * использованием банковского округления к числу, у которого не больше 7
 * значимых цифр.
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst);

/**
 * @brief Преобразование Decimal в целое число (int).
 *
 * @param src Исходное число типа Decimal.
 * @param dst Указатель на переменную для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CONVERSION (1).
 * @note Если в числе есть дробная часть, она отбрасывается (усечение к нулю,
 * например, 0.9 -> 0).
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst);

/**
 * @brief Преобразование Decimal в число с плавающей точкой (float).
 *
 * @param src Исходное число типа Decimal.
 * @param dst Указатель на переменную для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CONVERSION (1).
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst);

/* ======================================================================== */
/*                         ПРОЧИЕ ФУНКЦИИ                                   */
/* ======================================================================== */

/**
 * @brief Округляет Decimal до ближайшего целого числа в сторону отрицательной
 * бесконечности.
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CALCULATION (1).
 */
int s21_floor(s21_decimal value, s21_decimal *result);

/**
 * @brief Округляет Decimal до ближайшего целого числа (математическое
 * округление).
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CALCULATION (1).
 */
int s21_round(s21_decimal value, s21_decimal *result);

/**
 * @brief Возвращает целые цифры указанного Decimal числа.
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CALCULATION (1).
 * @note Любые дробные цифры отбрасываются (усечение к нулю).
 */
int s21_truncate(s21_decimal value, s21_decimal *result);

/**
 * @brief Инвертирует знак указанного Decimal числа.
 *
 * @param value Исходное число.
 * @param result Указатель на структуру для записи результата.
 * @return int S21_OK (0) или S21_ERROR_CALCULATION (1).
 */
int s21_negate(s21_decimal value, s21_decimal *result);

#endif  // S21_DECIMAL_H_