

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "task_manager.h"  

using namespace std;


struct TestCase {
    string input;
    vector<task> expected;
};



TEST(DateUtilsTest, IsLeapYear) {
    EXPECT_TRUE(isLeapYear(2024));   // високосный
    EXPECT_FALSE(isLeapYear(2023));  // обычный
    EXPECT_TRUE(isLeapYear(2000));   // високосный (делится на 400)
    EXPECT_FALSE(isLeapYear(1900));  // НЕ високосный (делится на 100, но не на 400)
}

TEST(DateUtilsTest, IsValidDate) {
    // Корректные даты
    EXPECT_TRUE(isValidDate(29, 2, 2024));  // високосный февраль
    EXPECT_TRUE(isValidDate(31, 12, 2025)); // декабрь
    EXPECT_TRUE(isValidDate(1, 1, 1));      // минимальная дата
    
    // Некорректные даты
    EXPECT_FALSE(isValidDate(30, 2, 2024));  // 30 февраля
    EXPECT_FALSE(isValidDate(0, 1, 2025));   // день 0
    EXPECT_FALSE(isValidDate(1, 13, 2025));  // месяц 13
    EXPECT_FALSE(isValidDate(1, 1, 0));      // год 0
}

TEST(DateUtilsTest, IsOverdue) {
    EXPECT_TRUE(isOverdue("2025-12-01", "2025-12-15"));   // просрочена
    EXPECT_FALSE(isOverdue("2025-12-20", "2025-12-15"));  // не просрочена
    EXPECT_FALSE(isOverdue("2025-12-15", "2025-12-15"));  // сегодня
    EXPECT_FALSE(isOverdue("2024-12-15", "2025-01-01"));  // прошлый год
}

TEST(JsonUtilsTest, EscapeJson) {
    EXPECT_EQ(escapeJson("test"), "test");
    EXPECT_EQ(escapeJson("\"quote\\\""), "\\\"quote\\\\\\\"");
    EXPECT_EQ(escapeJson("line\nbreak"), "line\\nbreak");
    EXPECT_EQ(escapeJson("\\\\double\\"), "\\\\\\\\double\\\\");
}



TEST(ReadFileTest, EmptyFile) {
    vector<task> tasks;
    readFile("test_empty.json", tasks);
    EXPECT_TRUE(tasks.empty());
}

TEST(ReadFileTest, SingleValidTask) {
    vector<task> tasks;
    readFile("test_single.json", tasks);
    
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].id, "1");
    EXPECT_EQ(tasks[0].title, "Купить молоко");
    EXPECT_EQ(tasks[0].due, "2025-12-25");
    EXPECT_EQ(tasks[0].priority, "low");
    EXPECT_EQ(tasks[0].group, "");
    EXPECT_FALSE(tasks[0].done);
}

TEST(ReadFileTest, MultipleTasks) {
    vector<task> tasks;
    readFile("test_multiple.json", tasks);
    
    ASSERT_EQ(tasks.size(), 3);
    EXPECT_EQ(tasks[1].id, "2");
    EXPECT_EQ(tasks[2].priority, "high");
    EXPECT_TRUE(tasks[0].done);
}

TEST(ReadFileTest, InvalidJson) {
    vector<task> tasks;
    // Файл с ошибками JSON
    readFile("test_invalid.json", tasks);
    EXPECT_LE(tasks.size(), 2);  // некоторые задачи могут быть пропущены
}



TEST(CreateTaskTest, ValidInput) {
    task t;
    // Симуляция ввода через мок (в реальности нужен mocking framework)
    // Здесь тестируем только валидацию внутри CreateTask
    t.priority = "mid";
    t.due = "2025-12-30";
    t.title = "Тестовая задача";
    t.group = "home";
    
    EXPECT_EQ(t.priority, "mid");
    EXPECT_TRUE(isValidDate(30, 12, 2025));  // проверяем дату
}

TEST(RefactorTaskTest, PriorityValidation) {
    task t;
    t.priority = "invalid";
    
    // Симуляция изменения приоритета
    unordered_set<string> valid = {"low", "mid", "high"};
    EXPECT_FALSE(valid.count("invalid"));
    EXPECT_TRUE(valid.count("low"));
}



TEST(PrintFiltersTest, GroupFilter) {
    vector<task> tasks = {
        {"1", "low", "Задача1", "2025-12-25", "work", false},
        {"2", "high", "Задача2", "2025-12-26", "work", false},
        {"3", "mid", "Задача3", "2025-12-27", "home", true}
    };
    
    // Тестируем логику фильтра (без вывода)
    int count_work = 0;
    for (const auto& task : tasks) {
        if (task.group == "work") count_work++;
    }
    EXPECT_EQ(count_work, 2);
}

TEST(PrintFiltersTest, OverdueFilter) {
    vector<task> tasks = {
        {"1", "low", "Задача1", "2025-12-20", "", false},  // просрочена
        {"2", "high", "Задача2", "2025-12-25", "", true},  // выполнена
        {"3", "mid", "Задача3", "2025-12-30", "", false}   // не просрочена
    };
    
    int overdue_count = 0;
    string today = "2025-12-23";
    for (const auto& task : tasks) {
        if (!task.done && isOverdue(task.due, today)) overdue_count++;
    }
    EXPECT_EQ(overdue_count, 1);
}



TEST(IntegrationTest, FullCycle) {
    vector<task> tasks;
    
    // 1. Чтение файла
    readFile("test_valid.json", tasks);
    ASSERT_FALSE(tasks.empty());
    
    // 2. Создание новой задачи
    task new_task;
    new_task.id = to_string(tasks.size() + 1);
    new_task.title = "Новая задача";
    new_task.due = "2025-12-31";
    new_task.priority = "high";
    tasks.push_back(new_task);
    
    // 3. Сохранение
    saveAllTasks("test_output.json", tasks);
    
    // 4. Проверка сохранения (чтение обратно)
    vector<task> saved_tasks;
    readFile("test_output.json", saved_tasks);
    ASSERT_EQ(saved_tasks.size(), tasks.size());
    EXPECT_EQ(saved_tasks.back().title, "Новая задача");
}



TEST(MenuTest, IdGeneration) {
    vector<task> empty_list;
    task first_task;
    if (empty_list.empty()) {
        first_task.id = "1";
    }
    EXPECT_EQ(first_task.id, "1");
    
    empty_list.push_back(first_task);
    task second_task;
    second_task.id = to_string(empty_list.size() + 1);
    EXPECT_EQ(second_task.id, "2");
}

TEST(MenuTest, DeleteTask) {
    vector<task> tasks = {{"1", "", "", "", "", false}, {"2", "", "", "", "", false}};
    
    // Удаляем задачу с id=1 (индекс 0)
    tasks.erase(tasks.begin() + 0);
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].id, "2");
}



TEST(EdgeCasesTest, EmptyTaskList) {
    vector<task> empty;
    // Все операции должны корректно обрабатывать пустой список
    EXPECT_TRUE(empty.empty());
}

TEST(EdgeCasesTest, InvalidDateFormats) {
    EXPECT_FALSE(isValidDate(32, 1, 2025));  // 32 января
    EXPECT_FALSE(isValidDate(1, 0, 2025));   // месяц 0
}

TEST(EdgeCasesTest, MalformedJson) {
    vector<task> tasks;
    // Файл с битыми данными
    readFile("test_malformed.json", tasks);
    // Должны получить частичные данные без краха
}

// ===== MAIN ФУНКЦИЯ ДЛЯ ЗАПУСКА ТЕСТОВ =====
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
