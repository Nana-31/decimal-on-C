/**
 * @file s21_helpers.c
 * @brief Реализация внутренних вспомогательных функций для работы с
 * s21_decimal.
 */

#include "s21_helpers.h"

/* ======================================================================== */
/*               ИНИЦИАЛИЗАЦИЯ И РАБОТА СО СВОЙСТВАМИ (БИТАМИ)              */
/* ======================================================================== */

/**
 * @brief Обнуляет все биты числа.
 * @param value Указатель на число для обнуления.
 */
void s21_decimal_zero(s21_decimal *value) {
  if (value) {
    value->bits[0] = 0;
    value->bits[1] = 0;
    value->bits[2] = 0;
    value->bits[3] = 0;
  }
}

/**
 * @brief Получает значение указанного бита.
 * @param value Указатель на число.
 * @param bit_index Индекс бита (от 0 до 127).
 * @return int 1, если бит установлен, 0, если сброшен.
 */
int s21_get_bit(const s21_decimal *value, int bit_index) {
  int result = 0;
  if (value && bit_index >= 0 && bit_index <= 127) {
    int block = bit_index / 32;
    int offset = bit_index % 32;
    result = (value->bits[block] >> offset) & 1;
  }
  return result;
}

/**
 * @brief Устанавливает или сбрасывает указанный бит.
 * @param value Указатель на число.
 * @param bit_index Индекс бита (от 0 до 127).
 * @param bit_value Новое значение бита (0 или 1).
 */
void s21_set_bit(s21_decimal *value, int bit_index, int bit_value) {
  if (value && bit_index >= 0 && bit_index <= 127) {
    int block = bit_index / 32;
    int offset = bit_index % 32;
    if (bit_value) {
      value->bits[block] |= (1U << offset);
    } else {
      value->bits[block] &= ~(1U << offset);
    }
  }
}

/**
 * @brief Получает знак числа.
 * @param value Указатель на число.
 * @return int 1, если число отрицательное, 0, если положительное или ноль.
 */
int s21_get_sign(const s21_decimal *value) {
  int sign = 0;
  if (value) {
    sign = (value->bits[3] >> 31) & 1;
  }
  return sign;
}

/**
 * @brief Устанавливает знак числа.
 * @param value Указатель на число.
 * @param sign Значение знака (0 для +, 1 для -).
 */
void s21_set_sign(s21_decimal *value, int sign) {
  if (value) {
    if (sign) {
      value->bits[3] |= (1U << 31);
    } else {
      value->bits[3] &= ~(1U << 31);
    }
  }
}

/**
 * @brief Получает текущий масштаб (scale) числа.
 * @param value Указатель на число.
 * @return int Текущий scale (от 0 до 28).
 */
int s21_get_scale(const s21_decimal *value) {
  int scale = 0;
  if (value) {
    scale = (value->bits[3] >> 16) & 0xFF;
  }
  return scale;
}

/**
 * @brief Устанавливает масштаб (scale) числа.
 * @param value Указатель на число.
 * @param scale Новое значение scale (должно быть в диапазоне 0-28).
 */
void s21_set_scale(s21_decimal *value, int scale) {
  if (value && scale >= 0 && scale <= 28) {
    value->bits[3] &= ~(0xFF << 16);
    value->bits[3] |= ((uint32_t)scale << 16);
  }
}

/* ======================================================================== */
/*                    ПРОВЕРКИ И СРАВНЕНИЕ МОДУЛЕЙ                          */
/* ======================================================================== */

/**
 * @brief Проверяет, равно ли число нулю.
 * @param value Указатель на число.
 * @return int 1, если число равно 0, иначе 0.
 */
int s21_is_zero(const s21_decimal *value) {
  int result = 0;
  if (value) {
    result =
        (value->bits[0] == 0 && value->bits[1] == 0 && value->bits[2] == 0);
  }
  return result;
}

/**
 * @brief Сравнивает модули двух чисел, игнорируя знак.
 * @param a Указатель на первое число.
 * @param b Указатель на второе число.
 * @return int 1, если |a| > |b|; -1, если |a| < |b|; 0, если |a| == |b|.
 */
int s21_bigint_cmp_abs(const s21_decimal *a, const s21_decimal *b) {
  int result = 0;
  if (a && b) {
    for (int i = 2; i >= 0; i--) {
      if (a->bits[i] > b->bits[i]) {
        result = 1;
        break;
      } else if (a->bits[i] < b->bits[i]) {
        result = -1;
        break;
      }
    }
  }
  return result;
}

/* ======================================================================== */
/*               БАЗОВАЯ АРИФМЕТИКА 96-БИТНЫХ МАНТИСС (BIGINT)              */
/* ======================================================================== */

/**
 * @brief Складывает мантиссы двух чисел как беззнаковые 96-битные целые числа.
 */
int s21_bigint_add(const s21_decimal *a, const s21_decimal *b,
                   s21_decimal *result) {
  if (!a || !b || !result) return S21_ERROR_CALCULATION;
  uint64_t carry = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t sum = (uint64_t)a->bits[i] + (uint64_t)b->bits[i] + carry;
    result->bits[i] = (uint32_t)(sum & 0xFFFFFFFF);
    carry = sum >> 32;
  }
  result->bits[3] = 0;  // Обнуляем служебные биты
  return (carry > 0) ? S21_ERROR_TOO_LARGE : S21_OK;
}

/**
 * @brief Вычитает мантиссу b из мантиссы a как беззнаковые 96-битные числа.
 * @note Предполагается, что |a| >= |b|.
 */
int s21_bigint_sub(const s21_decimal *a, const s21_decimal *b,
                   s21_decimal *result) {
  if (!a || !b || !result) return S21_ERROR_CALCULATION;
  int borrow = 0;
  for (int i = 0; i < 3; i++) {
    int64_t diff = (int64_t)a->bits[i] - (int64_t)b->bits[i] - borrow;
    if (diff < 0) {
      diff += 0x100000000LL;
      borrow = 1;
    } else {
      borrow = 0;
    }
    result->bits[i] = (uint32_t)diff;
  }
  result->bits[3] = 0;  // Обнуляем служебные биты
  return S21_OK;
}

/**
 * @brief Делит 96-битную мантиссу на 10, возвращая остаток.
 * @note Использует 64-битную арифметику для безопасного переноса остатка между
 * 32-битными блоками.
 * @param value Указатель на число (мантисса будет изменена).
 * @param remainder Указатель на переменную для записи остатка от деления (0-9).
 * @return int S21_OK при успехе, S21_ERROR_CALCULATION при ошибке (например,
 * NULL).
 */
int s21_div_mantissa_by_10(s21_decimal *value, int *remainder) {
  if (!value || !remainder) {
    return S21_ERROR_CALCULATION;
  }

  uint64_t rem = 0;
  // Делим от старшего блока к младшему
  for (int i = 2; i >= 0; i--) {
    uint64_t current = (rem << 32) | value->bits[i];
    value->bits[i] = (uint32_t)(current / 10);
    rem = current % 10;
  }
  *remainder = (int)rem;
  return S21_OK;
}

/* ======================================================================== */
/*                         БИТОВЫЕ СДВИГИ                                   */
/* ======================================================================== */

/**
 * @brief Сдвигает 96-битную мантиссу числа влево на указанное количество бит.
 * @note Знак и scale не изменяются. Биты, выходящие за пределы 96 бит,
 * теряются.
 */
void s21_bigint_shift_left(s21_decimal *value, int shift_bits) {
  if (!value || shift_bits <= 0) return;

  if (shift_bits >= 96) {
    // Если сдвиг >= 96 бит, обнуляем мантиссу.
    value->bits[0] = 0;
    value->bits[1] = 0;
    value->bits[2] = 0;
    return;
  }

  // Оптимизация: сдвиг на целое количество 32-битных блоков.
  int block_shift = shift_bits / 32;
  int bit_shift = shift_bits % 32;

  if (block_shift > 0) {
    // Сдвигаем блоки от старшего к младшему.
    for (int i = 2; i >= block_shift; i--) {
      value->bits[i] = value->bits[i - block_shift];
    }
    // Обнуляем младшие блоки.
    for (int i = 0; i < block_shift; i++) {
      value->bits[i] = 0;
    }
  }

  // Побитовый сдвиг внутри блоков (от старшего к младшему).
  if (bit_shift > 0) {
    uint32_t carry = 0;
    for (int i = 0; i < 3; i++) {
      uint64_t temp = ((uint64_t)value->bits[i] << bit_shift) | carry;
      value->bits[i] = (uint32_t)temp;
      carry = (uint32_t)(temp >> 32);
    }
    // Если carry != 0 после обработки последнего блока, биты теряются
    // (игнорируем)
  }
}

/**
 * @brief Сдвигает 96-битную мантиссу числа вправо на указанное количество бит.
 * @note Знак и scale не изменяются. Освобождающиеся старшие биты заполняются
 * нулями.
 */
void s21_bigint_shift_right(s21_decimal *value, int shift_bits) {
  if (!value || shift_bits <= 0) return;

  if (shift_bits >= 96) {
    // Если сдвиг >= 96 бит, обнуляем мантиссу.
    value->bits[0] = 0;
    value->bits[1] = 0;
    value->bits[2] = 0;
    return;
  }

  // Оптимизация: сдвиг на целое количество 32-битных блоков.
  int block_shift = shift_bits / 32;
  int bit_shift = shift_bits % 32;

  if (block_shift > 0) {
    // Сдвигаем блоки.
    for (int i = 0; i < 3 - block_shift; i++) {
      value->bits[i] = value->bits[i + block_shift];
    }
    // Обнуляем старшие блоки.
    for (int i = 3 - block_shift; i < 3; i++) {
      value->bits[i] = 0;
    }
  }

  // Побитовый сдвиг внутри блоков.
  if (bit_shift > 0) {
    for (int i = 0; i < 3; i++) {
      uint64_t temp = ((uint64_t)value->bits[i]) |
                      ((i < 2) ? ((uint64_t)value->bits[i + 1] << 32) : 0);
      value->bits[i] = (uint32_t)(temp >> bit_shift);
    }
  }
}

/* ======================================================================== */
/*               НОРМАЛИЗАЦИЯ И МАСШТАБИРОВАНИЕ (SCALE)                     */
/* ======================================================================== */

/**
 * @brief Приводит два числа к общему масштабу (scale).
 */
int s21_normalize(s21_decimal *a, s21_decimal *b) {
  if (!a || !b) return S21_ERROR_CALCULATION;

  int scale_a = s21_get_scale(a);
  int scale_b = s21_get_scale(b);

  // Умножаем число с меньшим scale на 10, пока scale не сравняются
  while (scale_a < scale_b) {
    if (s21_mul_mantissa_by_10(a) != S21_OK) {
      return S21_ERROR_CALCULATION;  // Переполнение при нормализации
    }
    scale_a = s21_get_scale(a);
  }

  while (scale_b < scale_a) {
    if (s21_mul_mantissa_by_10(b) != S21_OK) {
      return S21_ERROR_CALCULATION;
    }
    scale_b = s21_get_scale(b);
  }

  return S21_OK;
}

/**
 * @brief Умножает мантиссу числа на 10 и увеличивает его scale на 1.
 */
int s21_mul_mantissa_by_10(s21_decimal *value) {
  if (!value) return S21_ERROR_CALCULATION;

  int scale = s21_get_scale(value);
  if (scale >= 28) return S21_ERROR_TOO_LARGE;  // Превышен максимальный scale

  uint64_t carry = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t temp = (uint64_t)value->bits[i] * 10 + carry;
    value->bits[i] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;
  }

  if (carry > 0) {
    return S21_ERROR_TOO_LARGE;  // Переполнение 96-битной мантиссы
  }

  s21_set_scale(value, scale + 1);
  return S21_OK;
}

/**
 * @brief Округляет число до заданного масштаба путем деления на 10.
 * @param value Указатель на число (будет изменено: scale уменьшится, мантисса
 * разделится).
 * @param target_scale Целевой масштаб (0 <= target_scale <= 28).
 * @param mode Режим округления (S21_ROUND_TRUNCATE, S21_ROUND_HALF_UP,
 * S21_ROUND_HALF_EVEN).
 * @return int S21_OK при успехе, S21_ERROR_CALCULATION при некорректных
 * параметрах.
 */
int s21_round_to_scale(s21_decimal *value, int target_scale,
                       s21_rounding_mode mode) {
  if (!value || target_scale < 0 || target_scale > 28) {
    return S21_ERROR_CALCULATION;
  }

  int current_scale = s21_get_scale(value);

  while (current_scale > target_scale) {
    int remainder = 0;
    s21_div_mantissa_by_10(value, &remainder);
    current_scale--;

    int round_up = 0;

    if (mode == S21_ROUND_HALF_UP) {
      if (remainder >= 5) {
        round_up = 1;
      }
    } else if (mode == S21_ROUND_HALF_EVEN) {
      if (remainder > 5) {
        round_up = 1;
      } else if (remainder == 5) {
        // Банковское округление: если остаток 5, округляем к ближайшему
        // четному. Проверяем, является ли текущий младший бит мантиссы
        // нечетным.
        if (value->bits[0] % 2 != 0) {
          round_up = 1;
        }
      }
    }
    // Для S21_ROUND_TRUNCATE round_up остается 0

    if (round_up) {
      // Простое добавление 1 к 96-битной мантиссе
      uint64_t sum = (uint64_t)value->bits[0] + 1;
      value->bits[0] = (uint32_t)sum;
      if (sum > 0xFFFFFFFF) {
        sum = (uint64_t)value->bits[1] + 1;
        value->bits[1] = (uint32_t)sum;
        if (sum > 0xFFFFFFFF) {
          value->bits[2] += 1;
        }
      }
    }
  }

  s21_set_scale(value, current_scale);
  return S21_OK;
}

/* ======================================================================== */
/*               ФУНКЦИИ ДЛЯ РАБОТЫ С S21_BIG_DECIMAL                    */
/* ======================================================================== */

// Используется для преобразования из s21_decimal в s21_big_decimal
void s21_to_big_decimal(s21_decimal src, s21_big_decimal *dst) {
  for (int i = 0; i < 6; i++) {
    dst->bits[i] = 0;
  }
  // Копируем мантиссу, приводя к беззнаковому типу для безопасности
  dst->bits[0] = (uint32_t)src.bits[0];
  dst->bits[1] = (uint32_t)src.bits[1];
  dst->bits[2] = (uint32_t)src.bits[2];

  dst->scale = s21_get_scale(&src);
  dst->sign = s21_get_sign(&src);
}

// умножение s21_big_decimal на 10
void s21_mul_big_mantissa_by_10(s21_big_decimal *value) {
  uint64_t carry = 0;
  for (int i = 0; i < 6; i++) {
    uint64_t temp = (uint64_t)value->bits[i] * 10 + carry;
    value->bits[i] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;
  }
}

// нормализация (приведение к 1 scale) для s21_big_decimal
void s21_normalize_big(s21_big_decimal *a, s21_big_decimal *b) {
  // Просто умножаем на 10, переполнение нам больше не страшно!
  while (a->scale < b->scale) {
    s21_mul_big_mantissa_by_10(a);
    a->scale++;
  }
  while (b->scale < a->scale) {
    s21_mul_big_mantissa_by_10(b);
    b->scale++;
  }
}

// Сравнение модулей 2 чисел s21_big_decimal
int s21_cmp_big_mantissa(const s21_big_decimal *a, const s21_big_decimal *b) {
  int result = 0;
  int found = 0;

  for (int i = 5; i >= 0 && !found; i--) {
    if (a->bits[i] > b->bits[i]) {
      result = 1;
      found = 1;
    } else if (a->bits[i] < b->bits[i]) {
      result = -1;
      found = 1;
    }
  }

  return result;
}
