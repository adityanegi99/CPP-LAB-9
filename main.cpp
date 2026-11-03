#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <limits>

using namespace std;

// Smart Digital Expense Tracker
// Stores user accounts and expenses in local text files.

struct User {
    string username;
    string password;
};

struct Expense {
    int id;
    string username;
    string date;       // YYYY-MM-DD
    string category;
    string description;
    double amount;
};

const string USER_FILE = "users.txt";
const string EXPENSE_FILE = "expenses.txt";
const string BUDGET_FILE = "budgets.txt";

string cleanField(string value) {
    // Keep the pipe-delimited data files safe and simple.
    replace(value.begin(), value.end(), '|', '/');
    return value;
}

string currentDate() {
    time_t now = time(nullptr);
    tm *local = localtime(&now);
    ostringstream out;
    out << put_time(local, "%Y-%m-%d");
    return out.str();
}

string currentMonth() {
    return currentDate().substr(0, 7);
}

bool validDate(const string &date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(static_cast<unsigned char>(date[i]))) return false;
    }
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

vector<string> split(const string &line, char delimiter) {
    vector<string> fields;
    string field;
    stringstream stream(line);
    while (getline(stream, field, delimiter)) fields.push_back(field);
    return fields;
}

vector<User> loadUsers() {
    vector<User> users;
    ifstream file(USER_FILE);
    string line;
    while (getline(file, line)) {
        auto fields = split(line, '|');
        if (fields.size() == 2) users.push_back({fields[0], fields[1]});
    }
    return users;
}

vector<Expense> loadExpenses() {
    vector<Expense> expenses;
    ifstream file(EXPENSE_FILE);
    string line;
    while (getline(file, line)) {
        auto f = split(line, '|');
        if (f.size() != 6) continue;
        try {
            expenses.push_back({stoi(f[0]), f[1], f[2], f[3], f[4], stod(f[5])});
        } catch (...) {
            // Ignore malformed records instead of stopping the program.
        }
    }
    return expenses;
}

void saveExpenses(const vector<Expense> &expenses) {
    ofstream file(EXPENSE_FILE);
    for (const auto &e : expenses) {
        file << e.id << '|' << e.username << '|' << e.date << '|'
             << cleanField(e.category) << '|' << cleanField(e.description)
             << '|' << fixed << setprecision(2) << e.amount << '\n';
    }
}

double readPositiveAmount(const string &prompt) {
    double amount;
    while (true) {
        cout << prompt;
        if (cin >> amount && amount > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return amount;
        }
        cout << "Please enter a valid amount greater than zero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int nextExpenseId(const vector<Expense> &expenses) {
    int nextId = 1;
    for (const auto &e : expenses) nextId = max(nextId, e.id + 1);
    return nextId;
}

bool registerUser() {
    auto users = loadUsers();
    string username, password;
    cout << "\n--- User Registration ---\nUsername: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    for (const auto &user : users) {
        if (user.username == username) {
            cout << "That username already exists. Please choose another.\n";
            return false;
        }
    }

    ofstream file(USER_FILE, ios::app);
    file << cleanField(username) << '|' << cleanField(password) << '\n';
    cout << "Registration successful. You can now log in.\n";
    return true;
}

bool login(string &loggedInUser) {
    string username, password;
    cout << "\n--- Login ---\nUsername: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    for (const auto &user : loadUsers()) {
        if (user.username == username && user.password == password) {
            loggedInUser = username;
            cout << "Login successful. Welcome, " << username << "!\n";
            return true;
        }
    }
    cout << "Incorrect username or password.\n";
    return false;
}

double getMonthlyBudget(const string &username, const string &month) {
    ifstream file(BUDGET_FILE);
    string line;
    while (getline(file, line)) {
        auto f = split(line, '|');
        if (f.size() == 3 && f[0] == username && f[1] == month) {
            try { return stod(f[2]); } catch (...) { return 0; }
        }
    }
    return 0;
}

void setBudget(const string &username) {
    string month;
    cout << "Enter month (YYYY-MM), or press Enter for " << currentMonth() << ": ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, month);
    if (month.empty()) month = currentMonth();
    if (month.size() != 7 || month[4] != '-') {
        cout << "Invalid month format. Use YYYY-MM.\n";
        return;
    }

    double amount = readPositiveAmount("Enter monthly budget: Rs. ");
    vector<string> lines;
    ifstream in(BUDGET_FILE);
    string line;
    while (getline(in, line)) {
        auto f = split(line, '|');
        if (f.size() == 3 && !(f[0] == username && f[1] == month))
            lines.push_back(line);
    }
    ofstream out(BUDGET_FILE);
    for (const auto &saved : lines) out << saved << '\n';
    out << cleanField(username) << '|' << month << '|'
        << fixed << setprecision(2) << amount << '\n';
    cout << "Budget saved for " << month << ".\n";
}

double monthlyTotal(const vector<Expense> &expenses, const string &username,
                    const string &month) {
    double total = 0;
    for (const auto &e : expenses)
        if (e.username == username && e.date.substr(0, 7) == month)
            total += e.amount;
    return total;
}

void budgetStatus(const vector<Expense> &expenses, const string &username,
                  const string &month) {
    double budget = getMonthlyBudget(username, month);
    double spent = monthlyTotal(expenses, username, month);
    cout << fixed << setprecision(2);
    cout << "\nBudget report for " << month << "\n"
         << "Total spent : Rs. " << spent << '\n';
    if (budget <= 0) {
        cout << "No budget has been set for this month.\n";
        return;
    }
    cout << "Budget      : Rs. " << budget << '\n'
         << "Remaining   : Rs. " << budget - spent << '\n';
    if (spent > budget) cout << "WARNING: You have exceeded your monthly budget!\n";
    else if (spent >= budget * 0.80) cout << "Warning: You have used at least 80% of your budget.\n";
    else cout << "You are within your budget.\n";
}

void addExpense(const string &username, vector<Expense> &expenses) {
    Expense e;
    e.id = nextExpenseId(expenses);
    e.username = username;
    cout << "\n--- Add Daily Expense ---\n";
    cout << "Date (YYYY-MM-DD), or press Enter for today (" << currentDate() << "): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, e.date);
    if (e.date.empty()) e.date = currentDate();
    if (!validDate(e.date)) {
        cout << "Invalid date. Expense was not added.\n";
        return;
    }

    cout << "Category (Food/Travel/Study/Shopping/Bills/Other): ";
    getline(cin, e.category);
    cout << "Description: ";
    getline(cin, e.description);
    if (e.category.empty()) e.category = "Other";
    e.category = cleanField(e.category);
    e.description = cleanField(e.description);
    e.amount = readPositiveAmount("Amount (Rs.): ");

    expenses.push_back(e);
    saveExpenses(expenses);
    cout << "Expense added successfully. Expense ID: " << e.id << '\n';
    budgetStatus(expenses, username, e.date.substr(0, 7));
}

void printExpense(const Expense &e) {
    cout << left << setw(6) << e.id << setw(13) << e.date
         << setw(16) << e.category << setw(25) << e.description
         << "Rs. " << fixed << setprecision(2) << e.amount << '\n';
}

void viewHistory(const vector<Expense> &expenses, const string &username) {
    cout << "\n--- Expense History ---\n";
    cout << left << setw(6) << "ID" << setw(13) << "Date"
         << setw(16) << "Category" << setw(25) << "Description" << "Amount\n";
    cout << string(75, '-') << '\n';
    bool found = false;
    for (const auto &e : expenses) {
        if (e.username == username) { printExpense(e); found = true; }
    }
    if (!found) cout << "No expenses recorded yet.\n";
}

void categoryReport(const vector<Expense> &expenses, const string &username) {
    string month;
    cout << "Enter month (YYYY-MM), or press Enter for " << currentMonth() << ": ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, month);
    if (month.empty()) month = currentMonth();

    vector<pair<string, double>> totals;
    for (const auto &e : expenses) {
        if (e.username != username || e.date.substr(0, 7) != month) continue;
        auto it = find_if(totals.begin(), totals.end(),
                          [&](const pair<string,double> &p) { return p.first == e.category; });
        if (it == totals.end()) totals.push_back({e.category, e.amount});
        else it->second += e.amount;
    }
    sort(totals.begin(), totals.end(),
         [](const auto &a, const auto &b) { return a.second > b.second; });

    cout << "\nCategory-wise expenses for " << month << ":\n";
    if (totals.empty()) { cout << "No expenses found.\n"; return; }
    for (const auto &p : totals)
        cout << left << setw(18) << p.first << "Rs. " << fixed << setprecision(2) << p.second << '\n';
}

void monthlyReport(const vector<Expense> &expenses, const string &username) {
    string month;
    cout << "Enter month (YYYY-MM), or press Enter for " << currentMonth() << ": ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, month);
    if (month.empty()) month = currentMonth();

    double total = monthlyTotal(expenses, username, month);
    int count = 0;
    for (const auto &e : expenses)
        if (e.username == username && e.date.substr(0, 7) == month) ++count;

    cout << "\n--- Monthly Expense Report: " << month << " ---\n"
         << "Number of expenses: " << count << '\n'
         << "Total expense     : Rs. " << fixed << setprecision(2) << total << '\n';
    budgetStatus(expenses, username, month);
}

void predictExpense(const vector<Expense> &expenses, const string &username) {
    string month;
    cout << "Enter month (YYYY-MM), or press Enter for " << currentMonth() << ": ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, month);
    if (month.empty()) month = currentMonth();
    if (month.size() != 7 || month[4] != '-') {
        cout << "Invalid month format. Use YYYY-MM.\n";
        return;
    }

    double spent = monthlyTotal(expenses, username, month);
    double budget = getMonthlyBudget(username, month);
    int daysInMonth = 30;
    try {
        int year = stoi(month.substr(0, 4));
        int m = stoi(month.substr(5, 2));
        if (m < 1 || m > 12) throw invalid_argument("month");
        const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        daysInMonth = days[m - 1];
        bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        if (m == 2 && leap) daysInMonth = 29;
    } catch (...) {
        cout << "Invalid month format. Use YYYY-MM.\n";
        return;
    }

    int elapsedDays = 0;
    if (month == currentMonth()) {
        elapsedDays = stoi(currentDate().substr(8, 2));
    } else {
        // For a past month, the observed total is the final total.
        elapsedDays = daysInMonth;
    }

    double predicted = elapsedDays > 0 ? (spent / elapsedDays) * daysInMonth : 0;
    cout << fixed << setprecision(2)
         << "\n--- Expense Prediction for " << month << " ---\n"
         << "Spent so far       : Rs. " << spent << '\n'
         << "Predicted monthly  : Rs. " << predicted << '\n';
    if (budget > 0) {
        cout << "Monthly budget     : Rs. " << budget << '\n';
        if (predicted > budget) cout << "Prediction: You may exceed your budget.\n";
        else cout << "Prediction: You are likely to stay within your budget.\n";
    } else {
        cout << "Set a monthly budget to compare the prediction.\n";
    }
}

void searchExpense(const vector<Expense> &expenses, const string &username) {
    string keyword;
    cout << "Enter keyword (date, category or description): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, keyword);
    transform(keyword.begin(), keyword.end(), keyword.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });

    bool found = false;
    for (const auto &e : expenses) {
        if (e.username != username) continue;
        string searchable = e.date + " " + e.category + " " + e.description;
        transform(searchable.begin(), searchable.end(), searchable.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        if (searchable.find(keyword) != string::npos) {
            if (!found) cout << "\nMatching expenses:\n";
            printExpense(e);
            found = true;
        }
    }
    if (!found) cout << "No matching expense found.\n";
}

void deleteExpense(vector<Expense> &expenses, const string &username) {
    int id;
    cout << "Enter expense ID to delete: ";
    if (!(cin >> id)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    auto it = find_if(expenses.begin(), expenses.end(),
                      [&](const Expense &e) { return e.id == id && e.username == username; });
    if (it == expenses.end()) {
        cout << "Expense ID not found.\n";
        return;
    }
    expenses.erase(it);
    saveExpenses(expenses);
    cout << "Expense deleted successfully.\n";
}

void userMenu(const string &username) {
    vector<Expense> expenses = loadExpenses();
    int choice;
    do {
        cout << "\n========== SMART DIGITAL EXPENSE TRACKER ==========\n"
             << "Logged in as: " << username << "\n"
             << "1. Add Daily Expense\n"
             << "2. View Expense History\n"
             << "3. Category-wise Expense\n"
             << "4. Monthly Expense Report\n"
             << "5. Set Monthly Budget\n"
             << "6. Budget Warning / Status\n"
             << "7. Expense Prediction\n"
             << "8. Search Expense\n"
             << "9. Delete Expense\n"
             << "0. Logout\n"
             << "Enter your choice: ";
        if (!(cin >> choice)) {
            cout << "Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: addExpense(username, expenses); break;
            case 2: viewHistory(expenses, username); break;
            case 3: categoryReport(expenses, username); break;
            case 4: monthlyReport(expenses, username); break;
            case 5: setBudget(username); break;
            case 6: budgetStatus(expenses, username, currentMonth()); break;
            case 7: predictExpense(expenses, username); break;
            case 8: searchExpense(expenses, username); break;
            case 9: deleteExpense(expenses, username); break;
            case 0: cout << "Logged out successfully.\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}

int main() {
    cout << "===============================================\n"
         << "       SMART DIGITAL EXPENSE TRACKER\n"
         << "===============================================\n";
    int choice;
    do {
        cout << "\n1. Register\n2. Login\n0. Exit\nEnter your choice: ";
        if (!(cin >> choice)) {
            cout << "Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (choice == 1) registerUser();
        else if (choice == 2) {
            string username;
            if (login(username)) userMenu(username);
        } else if (choice == 0) cout << "Thank you for using the Expense Tracker.\n";
        else cout << "Invalid choice. Please try again.\n";
    } while (choice != 0);
    return 0;
}
