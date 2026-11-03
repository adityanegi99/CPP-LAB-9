# Smart Digital Expense Tracker

**Lab Sheet-09 | BCA I Semester | Session 2026–2027**

A menu-driven C++ console application for recording daily expenses, grouping spending by category, setting monthly budgets, warning users when spending is high, and estimating month-end expenses.

## Features

- User registration and login
- Add daily expenses with date, category, description, and amount
- View expense history
- Category-wise expense summary (highest spending first)
- Monthly expense report
- Set a budget for a selected month
- Budget warning at 80% usage and when the budget is exceeded
- Simple month-end expense prediction based on average daily spending
- Search expenses by date, category, or description
- Delete an expense by its ID
- Save users, expenses, and budgets in local text files

## Requirements

- A C++17 compiler (G++, MinGW, or an IDE with C++17 support)
- Terminal / command prompt

## Compile and run

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o expense_tracker
./expense_tracker
```

### Windows (MinGW)
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o expense_tracker.exe
expense_tracker.exe
```

The program creates these files in the current working directory when needed:

| File | Purpose |
|---|---|
| `users.txt` | Registered usernames and passwords |
| `expenses.txt` | Expense records |
| `budgets.txt` | Monthly budgets |

Keep these data files in the same directory from which you run the program. They are local runtime data and are intentionally excluded from Git.

## How prediction works

For the current month:

`Predicted monthly expense = (Total spent so far / Days elapsed) × Days in month`

For a past month, the recorded total is treated as the final total. This is a simple average-based estimate for a beginner lab project, not an AI model.

## Example workflow

1. Select **Register** and create an account.
2. Select **Login**.
3. Set a monthly budget, for example `2026-11`.
4. Add expenses and enter dates in `YYYY-MM-DD` format.
5. Open the monthly report or expense prediction to review spending.
6. Search or delete records when required.
7. Select **Logout**, then **Exit**.

## Notes and limitations

- Currency is displayed as **Rs.** and amounts are stored as decimal values.
- Enter dates as `YYYY-MM-DD` and months as `YYYY-MM`.
- Data is stored in plain text files on the local computer.
- This is an educational demonstration, not a production finance application. Passwords are stored as plain text; do not use real or reused passwords. A production version should use secure password hashing, stronger date validation, backups, and database storage.
- The estimate uses a daily average and does not account for future events or changing spending habits.

## Concepts used

Variables, loops, conditionals, arrays/vectors, structures, functions, strings, searching, sorting, and file handling.

## Lab report

### Objective
To develop a smart expense management system that helps users record daily expenses, categorize spending, monitor monthly budgets, and predict whether the monthly budget will be exceeded.

### Algorithm / logic
1. Display the main menu with Register, Login, and Exit options.
2. During registration, check that the username is not already present and save the new account.
3. During login, compare the entered credentials with the saved user records.
4. After successful login, display the expense-management menu.
5. For adding an expense, read its date, category, description, and positive amount; assign an ID and save the record.
6. For reports, filter records by the logged-in user and selected month, then calculate totals by month or category.
7. Compare monthly spending with the saved budget and display a warning at 80% usage or when the budget is exceeded.
8. Estimate month-end spending using average daily spending multiplied by the number of days in the month.
9. Search records using a keyword or delete a selected record by ID, then update the data file.
10. Continue until the user logs out or exits the program.

### Source code
See [`main.cpp`](main.cpp).

### Sample input and output
Example interaction (amounts and dates are illustrative):

```text
1. Register
2. Login
0. Exit
Enter your choice: 1

--- User Registration ---
Username: student1
Password: demo123
Registration successful. You can now log in.

Enter your choice: 2

--- Login ---
Username: student1
Password: demo123
Login successful. Welcome, student1!

1. Add Daily Expense
2. View Expense History
...
5. Set Monthly Budget
...
Enter your choice: 5
Enter month (YYYY-MM), or press Enter for 2026-11: 2026-11
Enter monthly budget: Rs. 5000
Budget saved for 2026-11.

Enter your choice: 1
Date (YYYY-MM-DD), or press Enter for today: 2026-11-02
Category (Food/Travel/Study/Shopping/Bills/Other): Food
Description: Lunch
Amount (Rs.): 150
Expense added successfully. Expense ID: 1

Budget report for 2026-11
Total spent : Rs. 150.00
Budget      : Rs. 5000.00
Remaining   : Rs. 4850.00
You are within your budget.
```

### Conclusion
The Smart Digital Expense Tracker provides a simple menu-driven way to manage personal expenses. It demonstrates user-defined structures, functions, searching, sorting, and file handling, while using monthly budget comparisons and an average-based prediction to help users monitor spending.
