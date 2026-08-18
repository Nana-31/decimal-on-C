/**
 * @file test_runner.c
 * @brief Точка входа для запуска unit-тестов библиотеки s21_decimal.
 *
 * Этот файл инициализирует тестовый раннер, добавляет все тестовые сьюты
 * (suites), реализации которых находятся в отдельных файлах, и выполняет
 * их запуск.
 */

#include <check.h>
#include <stdlib.h>

#include "../s21_decimal.h"

/* ======================================================================== */
/*                     ОБЪЯВЛЕНИЯ ТЕСТОВЫХ СЬЮТОВ                           */
/* ======================================================================== */

/**
 * @brief Объявление функции создания сьюта для внутренних хелперов.
 * @note Реализация находится в файле test_helpers.c.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_helpers_suite(void);

/**
 * @brief Объявление функции создания сьюта для сложения и вычитания.
 * @note Реализация находится в файле test_add_sub.c.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_add_sub_suite(void);

/**
 * @brief Объявление функции создания сьюта для умножения и деления.
 * @note Реализация находится в файле test_mul_div.c.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_mul_div_suite(void);

/**
 * @brief Объявление функции создания сьюта для операторов сравнения.
 * @note Реализация находится в файле test_comparison.c.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_comparison_suite(void);

/**
 * @brief Объявление функции создания сьюта для преобразователей типов.
 * @note Реализация находится в файле test_converters.c.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_converters_suite(void);

/**
 * @brief Объявление функции создания сьюта для функций округления и negate.
 * @note Реализация находится в файле test_rounding.c.
 * @return Suite* Указатель на созданный тестовый сьют.
 */
Suite *s21_rounding_suite(void);

/* ======================================================================== */
/*                           ТОЧКА ВХОДА                                    */
/* ======================================================================== */

/**
 * @brief Главная функция запуска всех unit-тестов.
 *
 * Инициализирует тестовый раннер, последовательно добавляет все
 * доступные тестовые сьюты и выполняет их с нормальным уровнем
 * детализации вывода (CK_NORMAL).
 *
 * @return int EXIT_SUCCESS (0), если все тесты пройдены успешно,
 *         иначе EXIT_FAILURE (1).
 */
int main(void)
{
    int number_failed;
    Suite *current_suite;

    // Создаем тестовый раннер и добавляем ПЕРВЫМ сьют хелперов (это фундамент)
    SRunner *test_runner = srunner_create(s21_helpers_suite());

    // Добавляем остальные тестовые сьюты в раннер
    current_suite = s21_add_sub_suite();
    srunner_add_suite(test_runner, current_suite);

    current_suite = s21_mul_div_suite();
    srunner_add_suite(test_runner, current_suite);

    current_suite = s21_comparison_suite();
    srunner_add_suite(test_runner, current_suite);

    current_suite = s21_converters_suite();
    srunner_add_suite(test_runner, current_suite);

    current_suite = s21_rounding_suite();
    srunner_add_suite(test_runner, current_suite);

    // Запускаем все тесты с нормальным уровнем вывода (CK_NORMAL)
    srunner_run_all(test_runner, CK_NORMAL);

    // Получаем количество проваленных тестов
    number_failed = srunner_ntests_failed(test_runner);

    // Освобождаем ресурсы, выделенные под тестовый раннер
    srunner_free(test_runner);

    // Возвращаем EXIT_SUCCESS, если все тесты пройдены, иначе EXIT_FAILURE
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}