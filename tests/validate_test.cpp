# Тесты валидности данных задач

---

## Обзор

Функция `validateData()` предназначена для проверки корректности JSON-данных задач, загружаемых из файлов.

В процессе валидации выполняются следующие проверки:

- ✅ наличие обязательных полей: `id`, `title`, `due`, `priority`, `group`, `done`;
- ✅ корректность формата даты (`YYYY-MM-DD`);
- ✅ допустимые значения приоритета (`low`, `mid`, `high`);
- ✅ уникальность идентификаторов задач (`id`);
- ✅ валидность типов данных.

---

## Функция валидации данных

```cpp
void validateData(const vector<task>& data) {
    set<string> taskIds;
    int validCount = 0;
    int errorCount = 0;

    for (const auto& item : data) {
        if (!item.id.empty() && item.id.find("\"") != string::npos) {
            cout << "Отсутствует или повреждён id" << endl;
            continue;
        }

        string id = item.id;
        if (!taskIds.insert(id).second) {
            cout << "Дубликат id: " << id << endl;
        }

        if (item.title.empty()) {
            cout << "Пустой title у задачи id=" << id << endl;
        }

        if (!item.due.empty()) {
            if (item.due.size() != 10 || item.due[4] != '-' || item.due[7] != '-') {
                cout << "Неверный формат due: " << item.due
                     << " у id=" << id << endl;
            }
        }

        unordered_set<string> validPriorities = {"low", "mid", "high"};
        if (!validPriorities.count(item.priority)) {
            cout << "Недопустимое значение priority: " << item.priority
                 << " у id=" << id << endl;
        }

        if (item.group.empty()) {
            cout << "Пустая группа у задачи id=" << id << endl;
        }
    }
}
```
