# ============================================================================
#                              КОНФИГУРАЦИЯ
# ============================================================================

# Компилятор и базовые флаги компиляции
CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror

# Флаги для генерации отчета покрытия кода (используются в цели gcov_report)
GCOV_FLAGS = -fprofile-arcs -ftest-coverage

# Флаги для проверки утечек памяти через Valgrind
VALGRIND_FLAGS = --tool=memcheck --leak-check=full --show-leak-kinds=all

# Структура проекта (Makefile находится внутри директории src/)
SRC_DIR = .
TEST_DIR = tests
OBJ_DIR = obj
OBJ_TEST_DIR = obj/tests

# Имена выходных файлов
LIBRARY = s21_decimal
STATIC_LIB = $(LIBRARY).a
SHARED_LIB = $(LIBRARY).so
TEST_BINARY = test_$(LIBRARY)
REPORT_DIR = report

# ============================================================================
#                           ИСХОДНЫЕ ФАЙЛЫ
# ============================================================================

# Все исходные файлы библиотеки (все .c в текущей директории)
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

# Все тестовые файлы (все .c в директории tests/)
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c, $(OBJ_TEST_DIR)/%.o, $(TEST_SRC))

# Зависимости для линковки тестов (библиотека Check и системные библиотеки)
TEST_LDFLAGS = -lcheck -lcheck_pic -pthread -lsubunit -lm

# ============================================================================
#                           СБОРКА БИБЛИОТЕКИ
# ============================================================================

# Цель по умолчанию: сборка статической и динамической библиотек
all: $(STATIC_LIB) $(SHARED_LIB)

# Создание статической библиотеки (архив объектных файлов)
$(STATIC_LIB): $(OBJ)
	ar rcs $@ $^
	ranlib $@

# Создание динамической библиотеки (shared object)
$(SHARED_LIB): $(OBJ)
	$(CC) -shared -o $@ $^

# Компиляция исходных файлов библиотеки в объектные файлы
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
#                           ТЕСТИРОВАНИЕ
# ============================================================================

# Сборка и запуск всех unit-тестов
test: $(TEST_BINARY)
	./$(TEST_BINARY)

# Линковка исполняемого файла тестов из объектных файлов
$(TEST_BINARY): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LDFLAGS)

# Компиляция тестовых файлов в объектные файлы
$(OBJ_TEST_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Проверка утечек памяти через Valgrind (CK_FORK=no для совместимости с Check)
valgrind: $(TEST_BINARY)
	CK_FORK=no valgrind $(VALGRIND_FLAGS) ./$(TEST_BINARY)

# ============================================================================
#                           ПОКРЫТИЕ КОДА
# ============================================================================

# Генерация HTML-отчета о покрытии кода тестами
gcov_report: clean
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(SRC) $(TEST_SRC) -o $(TEST_BINARY) $(TEST_LDFLAGS)
	./$(TEST_BINARY)
	mkdir -p $(REPORT_DIR)
	lcov --capture --directory . --output-file $(REPORT_DIR)/coverage.info \
	     --ignore-errors mismatch
	genhtml $(REPORT_DIR)/coverage.info --output-directory $(REPORT_DIR) \
	        --ignore-errors mismatch
	@echo ""
	@echo "Отчет о покрытии: $(REPORT_DIR)/index.html"

# ============================================================================
#                           ОЧИСТКА
# ============================================================================

# Удаление всех сгенерированных файлов и директорий
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(STATIC_LIB) $(SHARED_LIB) $(TEST_BINARY)
	rm -rf $(REPORT_DIR)
	rm -f *.gcno *.gcda *.gcov *.info
	rm -f *.out *.log

# Полная пересборка проекта (очистка + сборка)
rebuild: clean all

# ============================================================================
#                           СОЗДАНИЕ ДИРЕКТОРИЙ
# ============================================================================

# Создание директории для объектных файлов библиотеки
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Создание директории для объектных файлов тестов
$(OBJ_TEST_DIR):
	mkdir -p $(OBJ_TEST_DIR)

# ============================================================================
#                           СЛУЖЕБНЫЕ ЦЕЛИ
# ============================================================================

# Объявление целей, не являющихся файлами (для корректной работы make)
.PHONY: all clean rebuild test valgrind gcov_report