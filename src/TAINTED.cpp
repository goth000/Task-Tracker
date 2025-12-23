#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm> // для std::remove

using namespace std;

struct task {
    string id;
    string priority;
    string title;
    string due;
    string group;
    bool done = false;
};

// ===== Вспомогательные функции для дат =====
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isValidDate(int day, int month, int year) {
    if (year < 1 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month == 2 && isLeapYear(year)) {
        daysInMonth[1] = 29;
    }

    return day <= daysInMonth[month - 1];
}

// сравнение due (YYYY-MM-DD) с другой датой (YYYY-MM-DD)
// возвращает true, если due < today
bool isOverdue(const string& due, const string& today) {
    if (due.size() != 10 || today.size() != 10) return false;
    int y1 = stoi(due.substr(0, 4));
    int m1 = stoi(due.substr(5, 2));
    int d1 = stoi(due.substr(8, 2));

    int y2 = stoi(today.substr(0, 4));
    int m2 = stoi(today.substr(5, 2));
    int d2 = stoi(today.substr(8, 2));

    if (!isValidDate(d1, m1, y1) || !isValidDate(d2, m2, y2)) return false;

    if (y1 != y2) return y1 < y2;
    if (m1 != m2) return m1 < m2;
    return d1 < d2;
}

// ===== Экранирование строк для JSON =====
string escapeJson(const string& s) {
    string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    return out;
}

// Работа с JSON-файлом

// Ожидаемый формат data.json:
//
// [
//     {
//         "id": "1",
//         "title": "first",
//         "due": "2025-12-29",
//         "priority": "low",
//         "group": "",
//         "done": false
//     },
//     ...
// ]

void readFile(const string& name, vector<task>& list) {
    ifstream file(name);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для чтения!" << endl;
        return;
    }

    string line;
    task T;
    int counter = 0;
    int line_no = 0;
    bool anyTask = false;

    while (getline(file, line)) {
        ++line_no;

        // пропускаем строки с [ ], пустые и просто запятые
        if (line.find('{') != string::npos) {
            // начало объекта
            T = task();
            counter = 0;
            continue;
        }
        if (line.find('}') != string::npos) {
            // конец объекта
            anyTask = true;
            list.push_back(T);
            continue;
        }

        if (line.find("\"id\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": отсутствует ':' в поле id" << endl;
                continue;
            }
            int start = static_cast<int>(colon) + 2;
            string value = line.substr(start);
            value.erase(remove(value.begin(), value.end(), ' '), value.end());
            if (!value.empty() && value[0] == '\"') value.erase(0, 1);
            if (!value.empty() && value.back() == ',') value.pop_back();
            if (!value.empty() && value.back() == '\"') value.pop_back();
            T.id = value;
        }
        else if (line.find("\"title\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": отсутствует ':' в поле title" << endl;
                continue;
            }
            int start = static_cast<int>(colon) + 2;
            string value = line.substr(start);
            value.erase(remove(value.begin(), value.end(), ' '), value.end());
            if (!value.empty() && value[0] == '\"') value.erase(0, 1);
            if (!value.empty() && value.back() == ',') value.pop_back();
            if (!value.empty() && value.back() == '\"') value.pop_back();
            T.title = value;
        }
        else if (line.find("\"due\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": отсутствует ':' в поле due" << endl;
                continue;
            }
            int start = static_cast<int>(colon) + 2;
            string value = line.substr(start);
            value.erase(remove(value.begin(), value.end(), ' '), value.end());
            if (!value.empty() && value[0] == '\"') value.erase(0, 1);
            if (!value.empty() && value.back() == ',') value.pop_back();
            if (!value.empty() && value.back() == '\"') value.pop_back();
            T.due = value;
        }
        else if (line.find("\"priority\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": отсутствует ':' в поле priority" << endl;
                continue;
            }
            int start = static_cast<int>(colon) + 2;
            string value = line.substr(start);
            value.erase(remove(value.begin(), value.end(), ' '), value.end());
            if (!value.empty() && value[0] == '\"') value.erase(0, 1);
            if (!value.empty() && value.back() == ',') value.pop_back();
            if (!value.empty() && value.back() == '\"') value.pop_back();
            T.priority = value;
        }
        else if (line.find("\"group\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": отсутствует ':' в поле group" << endl;
                continue;
            }
            int start = static_cast<int>(colon) + 2;
            string value = line.substr(start);
            value.erase(remove(value.begin(), value.end(), ' '), value.end());
            if (!value.empty() && value[0] == '\"') value.erase(0, 1);
            if (!value.empty() && value.back() == ',') value.pop_back();
            if (!value.empty() && value.back() == '\"') value.pop_back();
            T.group = value;
        }
        else if (line.find("\"done\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": отсутствует ':' в поле done" << endl;
                continue;
            }
            int start = static_cast<int>(colon) + 1;
            string value = line.substr(start);
            value.erase(remove(value.begin(), value.end(), ' '), value.end());
            if (!value.empty() && value.back() == ',') value.pop_back();
            if (value != "true" && value != "false") {
                cerr << "Ошибка JSON в строке " << line_no
                    << ": ожидается true/false в поле done, найдено '" << value << "'" << endl;
            }
            T.done = (value == "true");
        }
    }

    if (!anyTask) {
        cerr << "Предупреждение: файл JSON прочитан, но задач не найдено." << endl;
    }

    file.close();
}

// Полная перезапись JSON-файла (используется после любых изменений)
void saveAllTasks(const string& name, const vector<task>& list) {
    ofstream file(name);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для записи!" << endl;
        return;
    }

    file << "[\n";
    for (size_t i = 0; i < list.size(); ++i) {
        const task& t = list[i];
        file << "    {\n";
        file << "        \"id\": \"" << escapeJson(t.id) << "\",\n";
        file << "        \"title\": \"" << escapeJson(t.title) << "\",\n";
        file << "        \"due\": \"" << escapeJson(t.due) << "\",\n";
        file << "        \"priority\": \"" << escapeJson(t.priority) << "\",\n";
        file << "        \"group\": \"" << escapeJson(t.group) << "\",\n";
        file << "        \"done\": " << (t.done ? "true" : "false") << "\n";
        file << "    }";
        if (i + 1 != list.size()) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
}

// ===== Работа с задачами =====

void PrintTask(const vector<task>& list) {
    for (const auto& elem : list) {
        cout << "Задача №" << elem.id << endl;
        cout << "Приоритет: " << elem.priority
            << " | Название: " << elem.title
            << " | Выполнить до: " << elem.due
            << " | Статус: " << (elem.done ? "Выполнена" : "Не выполнена")
            << " | Группа: " << elem.group << endl;
    }
}

// фильтр по группе
void PrintByGroup(const vector<task>& list, const string& group) {
    bool found = false;
    for (const auto& elem : list) {
        if (elem.group == group) {
            found = true;
            cout << "Задача №" << elem.id << endl;
            cout << "Приоритет: " << elem.priority
                << " | Название: " << elem.title
                << " | Выполнить до: " << elem.due
                << " | Статус: " << (elem.done ? "Выполнена" : "Не выполнена")
                << " | Группа: " << elem.group << endl;
        }
    }
    if (!found) {
        cout << "Задач в группе \"" << group << "\" не найдено." << endl;
    }
}

// отчёт о просроченных задачах
void PrintOverdue(const vector<task>& list, const string& today) {
    bool found = false;
    int count = 0;
    for (const auto& elem : list) {
        if (!elem.done && isOverdue(elem.due, today)) {
            found = true;
            count++;
            cout << "Задача №" << elem.id << endl;
            cout << "Приоритет: " << elem.priority
                << " | Название: " << elem.title
                << " | Выполнить до: " << elem.due
                << " | Статус: " << (elem.done ? "Выполнена" : "Не выполнена")
                << " | Группа: " << elem.group << endl;
        }
    }
    if (!found) {
        cout << "Просроченных невыполненных задач нет." << endl;
    }
    else {
        cout << "Всего просроченных задач: " << count << endl;
    }
}

bool checkAgree(bool& is_agree) {
    string temp;
    while (true) {
        cin >> temp;
        if (temp == "y" || temp == "n") {
            is_agree = (temp == "y");
            return true;
        }
        else {
            cout << "Вы неверно ввели, попробуйте еще раз (y/n)" << endl;
        }
    }
}

bool checkDone() {
    string temp;
    cout << "Введите: y для статуса выполнено и n для статуса не выполнено" << endl;
    while (true) {
        cin >> temp;
        if (temp == "y") {
            return true;
        }
        else if (temp == "n") {
            return false;
        }
        else {
            cout << "Вы неверно ввели, попробуйте еще раз (y/n)" << endl;
        }
    }
}

void CreateTask(task& T) {
    cout << "Добрый день! Вы выбрали пункт создать задачу, просто ответьте на вопросы" << endl;
    vector<string> args = { "Приоритет", "Название", "Дату исполнения", "Группу" };
    while (true) {
        bool another_try = false;
        int counter = 0;
        for (auto it = args.begin(); it != args.end(); ++it) {
            if (another_try) {
                --it;
                another_try = false;
            }
            counter++;
            cout << "Введите " << *it;
            if (*it == "Приоритет") {
                cout << " (low, mid, high)" << endl;
                string temp;
                cin >> temp;
                unordered_set<string> priority = { "low", "mid", "high" };
                if (!priority.count(temp)) {
                    cout << "Неверный приоритет, допустимо: low, mid, high" << endl;
                    another_try = true;
                    counter--;
                    continue;
                }
                T.priority = temp;
                continue;
            }
            if (*it == "Дату исполнения") {
                cout << " в виде YYYY-MM-DD" << endl;
                string temp;
                cin >> temp;
                if (temp.size() != 10 || temp[4] != '-' || temp[7] != '-') {
                    cout << "Неверный формат даты, попробуйте снова" << endl;
                    another_try = true;
                    counter--;
                    continue;
                }
                int year = stoi(temp.substr(0, 4));
                int month = stoi(temp.substr(5, 2));
                int day = stoi(temp.substr(8, 2));

                if (!isValidDate(day, month, year)) {
                    cout << "Некорректная дата, попробуйте снова" << endl;
                    another_try = true;
                    counter--;
                    continue;
                }
                T.due = temp;
                continue;
            }
            cout << endl;
            string temp;
            cin >> temp;
            if (*it == "Название") {
                T.title = temp;
            }
            else if (*it == "Группу") {
                T.group = temp;
            }
        }
        break;
    }
}

void RefactorTask(task& T) {
    cout << "Добрый день! Вы выбрали пункт изменить задачу." << endl;
    while (true) {
        bool is_agree = false;

        cout << "Хотите изменить приоритет? (y/n)" << endl;
        if (!checkAgree(is_agree)) return;
        if (is_agree) {
            cout << "Введите Приоритет (low, mid, high)" << endl;
            string temp;
            cin >> temp;
            unordered_set<string> priority = { "low", "mid", "high" };
            if (!priority.count(temp)) {
                cout << "Неверный приоритет, изменения не применены." << endl;
            }
            else {
                T.priority = temp;
            }
        }

        cout << "Хотите изменить дату исполнения? (y/n)" << endl;
        if (!checkAgree(is_agree)) return;
        if (is_agree) {
            cout << "Введите дату в виде YYYY-MM-DD" << endl;
            string temp;
            cin >> temp;
            if (temp.size() == 10 && temp[4] == '-' && temp[7] == '-') {
                int year = stoi(temp.substr(0, 4));
                int month = stoi(temp.substr(5, 2));
                int day = stoi(temp.substr(8, 2));

                if (!isValidDate(day, month, year)) {
                    cout << "Некорректная дата, изменения не применены." << endl;
                }
                else {
                    T.due = temp;
                }
            }
            else {
                cout << "Неверный формат даты, изменения не применены." << endl;
            }
        }

        cout << "Хотите изменить группу? (y/n)" << endl;
        if (!checkAgree(is_agree)) return;
        if (is_agree) {
            cout << "Введите Группу" << endl;
            string temp;
            cin >> temp;
            T.group = temp;
        }

        cout << "Хотите изменить название? (y/n)" << endl;
        if (!checkAgree(is_agree)) return;
        if (is_agree) {
            cout << "Введите Название" << endl;
            string temp;
            cin >> temp;
            T.title = temp;
        }

        cout << "Хотите изменить статус? (y/n)" << endl;
        if (!checkAgree(is_agree)) return;
        if (is_agree) {
            T.done = checkDone();
        }

        cout << "Хотите выйти из режима редактирования? (y/n)" << endl;
        if (!checkAgree(is_agree)) return;
        if (is_agree) {
            return;
        }
    }
}

// ===== Главное меню =====

void Start(vector<task>& list_of_tasks) {
    string filename = "data.json";
    int choose;
    while (true) {
        cout << "\nВведите число:" << endl;
        cout << "\t1 - создать задачу" << endl;
        cout << "\t2 - удалить задачу" << endl;
        cout << "\t3 - изменить задачу" << endl;
        cout << "\t4 - фильтр по группе" << endl;
        cout << "\t5 - отчет о просроченных задачах" << endl;
        cout << "\t6 - просмотр всех задач" << endl;
        cout << "\tЛюбой другой символ - выход" << endl;

        if (!(cin >> choose)) {
            cout << "Завершение работы." << endl;
            return;
        }

        switch (choose) {
        case 1: {
            task temp;
            if (list_of_tasks.empty()) {
                temp.id = "1";
            }
            else {
                temp.id = to_string(list_of_tasks.size() + 1);
            }
            CreateTask(temp);
            list_of_tasks.push_back(temp);
            saveAllTasks(filename, list_of_tasks);
            break;
        }
        case 2: {
            if (list_of_tasks.empty()) {
                cout << "Удалять нечего, попробуйте добавить что-то." << endl;
                break;
            }
            PrintTask(list_of_tasks);
            cout << "Введите номер задачи (id), которую нужно удалить" << endl;
            int pop = 0;
            cin >> pop;
            if (pop <= 0 || pop > (int)list_of_tasks.size()) {
                cout << "Введен неверный id задачи." << endl;
                break;
            }
            auto it = list_of_tasks.begin() + (pop - 1);
            list_of_tasks.erase(it);
            saveAllTasks(filename, list_of_tasks);
            cout << "Задача удалена." << endl;
            break;
        }
        case 3: {
            if (list_of_tasks.empty()) {
                cout << "Изменять нечего, попробуйте добавить что-то." << endl;
                break;
            }
            PrintTask(list_of_tasks);
            cout << "Введите номер задачи (id), которую нужно изменить" << endl;
            int pop = 0;
            cin >> pop;
            if (pop <= 0 || pop > (int)list_of_tasks.size()) {
                cout << "Введен неверный id задачи." << endl;
                break;
            }
            RefactorTask(list_of_tasks[pop - 1]);
            saveAllTasks(filename, list_of_tasks);
            cout << "Изменения сохранены." << endl;
            break;
        }
        case 4: {
            if (list_of_tasks.empty()) {
                cout << "Список задач пуст." << endl;
                break;
            }
            cout << "Введите название группы для фильтрации:" << endl;
            string grp;
            cin >> grp;
            PrintByGroup(list_of_tasks, grp);
            break;
        }
        case 5: {
            if (list_of_tasks.empty()) {
                cout << "Список задач пуст." << endl;
                break;
            }
            cout << "Для отчета о просроченных введите сегодняшнюю дату (YYYY-MM-DD):" << endl;
            string today;
            cin >> today;
            PrintOverdue(list_of_tasks, today);
            break;
        }
        case 6: {
            if (list_of_tasks.empty()) {
                cout << "Список задач пуст." << endl;
                break;
            }
            PrintTask(list_of_tasks);
            break;
        }
        default:
            cout << "Выход из программы." << endl;
            return;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Ru-ru");
    vector<task> list_of_tasks;
    string name = "data.json";

    readFile(name, list_of_tasks);
    Start(list_of_tasks);

    return 0;
}
