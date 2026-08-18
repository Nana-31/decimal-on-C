/**
 * @file test_helpers.c
 * @brief Unit-тесты для внутренних вспомогательных функций s21_decimal.
 *
 * @note Владелец файла: Тимлид. Хелперы — общий фундамент команды.
 *       Любой разработчик, обнаруживший баг, обязан добавить тест и
 *       предложить фикс через PR.
 */

#include <check.h>

#include "../s21_helpers.h"

/* ======================================================================== */
/*            ТЕСТЫ ДЛЯ БАЗОВЫХ ХЕЛПЕРОВ (БИТЫ, ЗНАК, SCALE)                */
/* ======================================================================== */

START_TEST(test_s21_decimal_zero_basic)
{
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};
    s21_decimal_zero(&value);
    ck_assert_uint_eq(value.bits[0], 0);
    ck_assert_uint_eq(value.bits[1], 0);
    ck_assert_uint_eq(value.bits[2], 0);
    ck_assert_uint_eq(value.bits[3], 0);
}
END_TEST

START_TEST(test_s21_decimal_zero_already_zero)
{
    s21_decimal value = {{0, 0, 0, 0}};
    s21_decimal_zero(&value);
    ck_assert_uint_eq(value.bits[0], 0);
}
END_TEST

START_TEST(test_s21_decimal_zero_null)
{
    s21_decimal_zero(NULL); // Не должно упасть
}
END_TEST

START_TEST(test_s21_get_set_bit_basic)
{
    s21_decimal value = {{0, 0, 0, 0}};

    s21_set_bit(&value, 0, 1);
    ck_assert_int_eq(s21_get_bit(&value, 0), 1);
    ck_assert_uint_eq(value.bits[0], 1);

    s21_set_bit(&value, 31, 1);
    ck_assert_int_eq(s21_get_bit(&value, 31), 1);
    ck_assert_uint_eq(value.bits[0], 0x80000001);

    s21_set_bit(&value, 32, 1);
    ck_assert_int_eq(s21_get_bit(&value, 32), 1);
    ck_assert_uint_eq(value.bits[1], 1);

    s21_set_bit(&value, 63, 1);
    ck_assert_int_eq(s21_get_bit(&value, 63), 1);

    s21_set_bit(&value, 64, 1);
    ck_assert_int_eq(s21_get_bit(&value, 64), 1);
    ck_assert_uint_eq(value.bits[2], 1);

    s21_set_bit(&value, 95, 1);
    ck_assert_int_eq(s21_get_bit(&value, 95), 1);

    s21_set_bit(&value, 0, 0);
    ck_assert_int_eq(s21_get_bit(&value, 0), 0);
}
END_TEST

START_TEST(test_s21_get_set_bit_boundaries)
{
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};

    for (int i = 0; i < 96; i++)
    {
        ck_assert_int_eq(s21_get_bit(&value, i), 1);
    }

    // Биты 96-127 (служебные) тоже должны работать
    ck_assert_int_eq(s21_get_bit(&value, 96), 1);
    ck_assert_int_eq(s21_get_bit(&value, 127), 1);

    // Некорректные индексы
    ck_assert_int_eq(s21_get_bit(&value, -1), 0);
    ck_assert_int_eq(s21_get_bit(&value, 128), 0);
}
END_TEST

START_TEST(test_s21_get_set_bit_null)
{
    ck_assert_int_eq(s21_get_bit(NULL, 0), 0);
    s21_set_bit(NULL, 0, 1); // Не должно упасть
}
END_TEST

START_TEST(test_s21_get_set_sign)
{
    s21_decimal value = {{0, 0, 0, 0}};

    ck_assert_int_eq(s21_get_sign(&value), 0);

    s21_set_sign(&value, 1);
    ck_assert_int_eq(s21_get_sign(&value), 1);
    ck_assert_uint_eq(value.bits[3] >> 31, 1);

    s21_set_sign(&value, 0);
    ck_assert_int_eq(s21_get_sign(&value), 0);
    ck_assert_uint_eq(value.bits[3] >> 31, 0);
}
END_TEST

START_TEST(test_s21_get_set_sign_null)
{
    ck_assert_int_eq(s21_get_sign(NULL), 0);
    s21_set_sign(NULL, 1); // Не должно упасть
}
END_TEST

START_TEST(test_s21_get_set_scale)
{
    s21_decimal value = {{0, 0, 0, 0}};

    ck_assert_int_eq(s21_get_scale(&value), 0);

    s21_set_scale(&value, 10);
    ck_assert_int_eq(s21_get_scale(&value), 10);

    s21_set_scale(&value, 28);
    ck_assert_int_eq(s21_get_scale(&value), 28);

    s21_set_scale(&value, 29);
    ck_assert_int_eq(s21_get_scale(&value), 28);

    s21_set_scale(&value, -1);
    ck_assert_int_eq(s21_get_scale(&value), 28);
}
END_TEST

START_TEST(test_s21_get_set_scale_null)
{
    ck_assert_int_eq(s21_get_scale(NULL), 0);
    s21_set_scale(NULL, 5); // Не должно упасть
}
END_TEST

START_TEST(test_s21_scale_sign_independence)
{
    // Проверяем, что scale и sign не затирают друг друга
    s21_decimal value = {{0, 0, 0, 0}};

    s21_set_scale(&value, 15);
    s21_set_sign(&value, 1);

    ck_assert_int_eq(s21_get_scale(&value), 15);
    ck_assert_int_eq(s21_get_sign(&value), 1);

    // Меняем scale — sign не должен измениться
    s21_set_scale(&value, 20);
    ck_assert_int_eq(s21_get_sign(&value), 1);
    ck_assert_int_eq(s21_get_scale(&value), 20);

    // Меняем sign — scale не должен измениться
    s21_set_sign(&value, 0);
    ck_assert_int_eq(s21_get_scale(&value), 20);
    ck_assert_int_eq(s21_get_sign(&value), 0);
}
END_TEST

/* ======================================================================== */
/*                    ТЕСТЫ ПРОВЕРОК И СРАВНЕНИЯ                            */
/* ======================================================================== */

START_TEST(test_s21_is_zero)
{
    s21_decimal zero = {{0, 0, 0, 0}};
    ck_assert_int_eq(s21_is_zero(&zero), 1);

    s21_decimal non_zero = {{1, 0, 0, 0}};
    ck_assert_int_eq(s21_is_zero(&non_zero), 0);

    s21_decimal signed_zero = {{0, 0, 0, 0}};
    s21_set_sign(&signed_zero, 1);
    s21_set_scale(&signed_zero, 5);
    ck_assert_int_eq(s21_is_zero(&signed_zero), 1);
}
END_TEST

START_TEST(test_s21_is_zero_null)
{
    ck_assert_int_eq(s21_is_zero(NULL), 0);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs)
{
    s21_decimal a = {{100, 0, 0, 0}};
    s21_decimal b = {{200, 0, 0, 0}};

    ck_assert_int_eq(s21_bigint_cmp_abs(&a, &b), -1);
    ck_assert_int_eq(s21_bigint_cmp_abs(&b, &a), 1);
    ck_assert_int_eq(s21_bigint_cmp_abs(&a, &a), 0);

    s21_decimal c = {{100, 0, 0, 0}};
    s21_set_sign(&c, 1);
    s21_set_scale(&c, 10);
    ck_assert_int_eq(s21_bigint_cmp_abs(&a, &c), 0);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs_multi_block)
{
    s21_decimal a = {{0, 1, 0, 0}};
    s21_decimal b = {{0xFFFFFFFF, 0, 0, 0}};

    ck_assert_int_eq(s21_bigint_cmp_abs(&a, &b), 1);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs_zeros)
{
    s21_decimal a = {{0, 0, 0, 0}};
    s21_decimal b = {{0, 0, 0, 0}};

    ck_assert_int_eq(s21_bigint_cmp_abs(&a, &b), 0);
}
END_TEST

START_TEST(test_s21_bigint_cmp_abs_null)
{
    s21_decimal a = {{100, 0, 0, 0}};

    ck_assert_int_eq(s21_bigint_cmp_abs(NULL, &a), 0);
    ck_assert_int_eq(s21_bigint_cmp_abs(&a, NULL), 0);
    ck_assert_int_eq(s21_bigint_cmp_abs(NULL, NULL), 0);
}
END_TEST

/* ======================================================================== */
/*               ТЕСТЫ ДЛЯ РЕАЛИЗОВАННЫХ АРИФМЕТИЧЕСКИХ ХЕЛПЕРОВ            */
/* ======================================================================== */

START_TEST(test_s21_div_mantissa_by_10_basic)
{
    s21_decimal value = {{100, 0, 0, 0}};
    int remainder = -1;

    int status = s21_div_mantissa_by_10(&value, &remainder);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 10);
    ck_assert_int_eq(remainder, 0);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_with_remainder)
{
    s21_decimal value = {{123, 0, 0, 0}};
    int remainder = -1;

    int status = s21_div_mantissa_by_10(&value, &remainder);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 12);
    ck_assert_int_eq(remainder, 3);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_max_remainder)
{
    // 19 / 10 = 1, остаток 9 (максимальный возможный)
    s21_decimal value = {{19, 0, 0, 0}};
    int remainder = -1;

    int status = s21_div_mantissa_by_10(&value, &remainder);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 1);
    ck_assert_int_eq(remainder, 9);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_zero)
{
    s21_decimal value = {{0, 0, 0, 0}};
    int remainder = -1;

    int status = s21_div_mantissa_by_10(&value, &remainder);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 0);
    ck_assert_int_eq(remainder, 0);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_multi_block)
{
    s21_decimal value = {{5, 1, 0, 0}};
    int remainder = -1;

    int status = s21_div_mantissa_by_10(&value, &remainder);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 0x1999999A);
    ck_assert_uint_eq(value.bits[1], 0);
    ck_assert_uint_eq(value.bits[2], 0);
    ck_assert_int_eq(remainder, 1);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_large_multi_block)
{
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    int remainder = -1;

    int status = s21_div_mantissa_by_10(&value, &remainder);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_ne(value.bits[2], 0);
    ck_assert_int_eq(remainder, 5);
}
END_TEST

START_TEST(test_s21_div_mantissa_by_10_null)
{
    s21_decimal value = {{100, 0, 0, 0}};
    int remainder;

    ck_assert_int_eq(s21_div_mantissa_by_10(NULL, &remainder), S21_ERROR_CALCULATION);
    ck_assert_int_eq(s21_div_mantissa_by_10(&value, NULL), S21_ERROR_CALCULATION);
}
END_TEST

START_TEST(test_s21_round_to_scale_truncate)
{
    s21_decimal value = {{12345, 0, 0, 0}};
    s21_set_scale(&value, 3);

    int status = s21_round_to_scale(&value, 1, S21_ROUND_TRUNCATE);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_up)
{
    s21_decimal value = {{1235, 0, 0, 0}};
    s21_set_scale(&value, 2);

    int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_UP);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 124);
    ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_up_down)
{
    s21_decimal value = {{1234, 0, 0, 0}};
    s21_set_scale(&value, 2);

    int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_UP);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_even)
{
    s21_decimal value = {{1235, 0, 0, 0}};
    s21_set_scale(&value, 2);

    int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_EVEN);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 124);
    ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_half_even_to_even)
{
    s21_decimal value = {{1225, 0, 0, 0}};
    s21_set_scale(&value, 2);

    int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_EVEN);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 122);
    ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_negative)
{
    // -12.35 с HALF_UP -> -12.4
    s21_decimal value = {{1235, 0, 0, 0}};
    s21_set_scale(&value, 2);
    s21_set_sign(&value, 1);

    int status = s21_round_to_scale(&value, 1, S21_ROUND_HALF_UP);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 124);
    ck_assert_int_eq(s21_get_sign(&value), 1);
    ck_assert_int_eq(s21_get_scale(&value), 1);
}
END_TEST

START_TEST(test_s21_round_to_scale_overflow)
{
    // 9.99 -> округление до scale=0 -> 10
    s21_decimal value = {{999, 0, 0, 0}};
    s21_set_scale(&value, 2);

    int status = s21_round_to_scale(&value, 0, S21_ROUND_HALF_UP);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 10);
    ck_assert_int_eq(s21_get_scale(&value), 0);
}
END_TEST

START_TEST(test_s21_round_to_scale_no_change)
{
    s21_decimal value = {{12345, 0, 0, 0}};
    s21_set_scale(&value, 3);

    int status = s21_round_to_scale(&value, 3, S21_ROUND_HALF_UP);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 12345);
    ck_assert_int_eq(s21_get_scale(&value), 3);
}
END_TEST

START_TEST(test_s21_round_to_scale_target_greater)
{
    // Если target > current, ничего не должно меняться
    s21_decimal value = {{123, 0, 0, 0}};
    s21_set_scale(&value, 2);

    int status = s21_round_to_scale(&value, 5, S21_ROUND_HALF_UP);

    ck_assert_int_eq(status, S21_OK);
    ck_assert_uint_eq(value.bits[0], 123);
    ck_assert_int_eq(s21_get_scale(&value), 2);
}
END_TEST

START_TEST(test_s21_round_to_scale_invalid)
{
    s21_decimal value = {{100, 0, 0, 0}};

    ck_assert_int_eq(s21_round_to_scale(NULL, 0, S21_ROUND_TRUNCATE), S21_ERROR_CALCULATION);
    ck_assert_int_eq(s21_round_to_scale(&value, -1, S21_ROUND_TRUNCATE), S21_ERROR_CALCULATION);
    ck_assert_int_eq(s21_round_to_scale(&value, 29, S21_ROUND_TRUNCATE), S21_ERROR_CALCULATION);
}
END_TEST

/* ======================================================================== */
/*         TODO: ТЕСТЫ ДЛЯ ХЕЛПЕРОВ, КОТОРЫЕ ЕЩЕ В ЗАГЛУШКАХ                */
/* ======================================================================== */

// TODO: Тимлид — реализуйте хелпер s21_bigint_add и добавьте тесты:
// - test_s21_bigint_add_basic (простое сложение без переноса)
// - test_s21_bigint_add_with_carry (с переносом разряда)
// - test_s21_bigint_add_overflow (переполнение 96 бит)
// - test_s21_bigint_add_multi_block (сложение с участием bits[1] и bits[2])
// - test_s21_bigint_add_zero (сложение с нулем)
// - test_s21_bigint_add_null (NULL-указатели)

// TODO: Тимлид — реализуйте хелпер s21_bigint_sub и добавьте тесты:
// - test_s21_bigint_sub_basic (простое вычитание)
// - test_s21_bigint_sub_with_borrow (с займом разряда)
// - test_s21_bigint_sub_equal (вычитание равных чисел -> 0)
// - test_s21_bigint_sub_underflow (вычитание большего из меньшего)
// - test_s21_bigint_sub_null (NULL-указатели)

// TODO: Тимлид — реализуйте хелпер s21_bigint_shift_left и добавьте тесты:
// - test_s21_bigint_shift_left_basic (сдвиг на 1 бит)
// - test_s21_bigint_shift_left_32 (сдвиг на 32 бита — переход между блоками)
// - test_s21_bigint_shift_left_64 (сдвиг на 64 бита)
// - test_s21_bigint_shift_left_overflow (сдвиг за пределы 96 бит)
// - test_s21_bigint_shift_left_zero (сдвиг нуля)
// - test_s21_bigint_shift_left_null (NULL-указатель)

// TODO: Тимлид — реализуйте хелпер s21_bigint_shift_right и добавьте тесты:
// - test_s21_bigint_shift_right_basic (сдвиг на 1 бит)
// - test_s21_bigint_shift_right_32 (сдвиг на 32 бита)
// - test_s21_bigint_shift_right_64 (сдвиг на 64 бита)
// - test_s21_bigint_shift_right_zero_fill (старшие биты заполняются нулями)
// - test_s21_bigint_shift_right_zero (сдвиг нуля)
// - test_s21_bigint_shift_right_null (NULL-указатель)

// TODO: Тимлид — реализуйте хелпер s21_mul_mantissa_by_10 и добавьте тесты:
// - test_s21_mul_mantissa_by_10_basic (простое умножение)
// - test_s21_mul_mantissa_by_10_with_scale (проверка увеличения scale)
// - test_s21_mul_mantissa_by_10_overflow (переполнение 96 бит)
// - test_s21_mul_mantissa_by_10_max_scale (ошибка при scale=28)
// - test_s21_mul_mantissa_by_10_zero (умножение нуля)
// - test_s21_mul_mantissa_by_10_null (NULL-указатель)

// TODO: Тимлид — реализуйте хелпер s21_normalize и добавьте тесты:
// - test_s21_normalize_equal_scales (scale уже равны)
// - test_s21_normalize_different_scales (выравнивание scale)
// - test_s21_normalize_overflow (переполнение мантиссы при нормализации)
// - test_s21_normalize_max_scale (ошибка при достижении scale=28)
// - test_s21_normalize_zero (нормализация с нулем)
// - test_s21_normalize_null (NULL-указатели)

/* ======================================================================== */
/*                         СБОРКА СЬЮТА (SUITE)                             */
/* ======================================================================== */

Suite *s21_helpers_suite(void)
{
    Suite *s = suite_create("s21_helpers");
    TCase *tc_basic = tcase_create("Basic");
    TCase *tc_arithmetic = tcase_create("Arithmetic");

    // Тесты базовых хелперов
    tcase_add_test(tc_basic, test_s21_decimal_zero_basic);
    tcase_add_test(tc_basic, test_s21_decimal_zero_already_zero);
    tcase_add_test(tc_basic, test_s21_decimal_zero_null);
    tcase_add_test(tc_basic, test_s21_get_set_bit_basic);
    tcase_add_test(tc_basic, test_s21_get_set_bit_boundaries);
    tcase_add_test(tc_basic, test_s21_get_set_bit_null);
    tcase_add_test(tc_basic, test_s21_get_set_sign);
    tcase_add_test(tc_basic, test_s21_get_set_sign_null);
    tcase_add_test(tc_basic, test_s21_get_set_scale);
    tcase_add_test(tc_basic, test_s21_get_set_scale_null);
    tcase_add_test(tc_basic, test_s21_scale_sign_independence);
    tcase_add_test(tc_basic, test_s21_is_zero);
    tcase_add_test(tc_basic, test_s21_is_zero_null);
    tcase_add_test(tc_basic, test_s21_bigint_cmp_abs);
    tcase_add_test(tc_basic, test_s21_bigint_cmp_abs_multi_block);
    tcase_add_test(tc_basic, test_s21_bigint_cmp_abs_zeros);
    tcase_add_test(tc_basic, test_s21_bigint_cmp_abs_null);

    // Тесты арифметических хелперов
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_basic);
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_with_remainder);
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_max_remainder);
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_zero);
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_multi_block);
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_large_multi_block);
    tcase_add_test(tc_arithmetic, test_s21_div_mantissa_by_10_null);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_truncate);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_up);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_up_down);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_even);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_half_even_to_even);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_negative);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_overflow);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_no_change);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_target_greater);
    tcase_add_test(tc_arithmetic, test_s21_round_to_scale_invalid);

    suite_add_tcase(s, tc_basic);
    suite_add_tcase(s, tc_arithmetic);

    return s;
}