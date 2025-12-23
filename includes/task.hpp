#pragma once
#include <string>
#include <stdexcept>

enum class Priority { Low, Mid, High };

struct Task {
    int id;
    std::string title;
    std::string due;  // YYYY-MM-DD
    Priority priority;
    std::string group;
    bool done;
};

Priority parse_priority(const std::string& s);
std::string priority_to_string(Priority p);
bool is_valid_date(const std::string& iso_date);
void validate_task(const Task& t);

struct TaskValidationError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
