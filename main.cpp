#include "EmployeeManager.h"
#include "Employee.h"

#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include <limits>

using namespace std;

int readHiddenChar() {
#ifdef _WIN32
    return _getch();
#else
    termios oldSettings{}, newSettings{};
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= static_cast<unsigned long>(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    return ch;
#endif
}

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

namespace Color {
    const string reset = "\033[0m";
    const string red = "\033[31m";
    const string green = "\033[32m";
    const string yellow = "\033[33m";
    const string blue = "\033[34m";
    const string cyan = "\033[36m";
    const string bold = "\033[1m";
}

string readPassword(const string& prompt) {
    cout << prompt;
    string password;
    while (true) {
        int ch = readHiddenChar();
        if (ch == '\r' || ch == '\n') {
            cout << '\n';
            break;
        }
        if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else if (ch == 0 || ch == 224) {
            (void)readHiddenChar();
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(static_cast<char>(ch));
            cout << '*';
        }
    }
    return password;
}

void pauseScreen() {
    cout << Color::yellow << "Nhan Enter de tiep tuc..." << Color::reset;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool askContinue() {
    char answer;
    while (true) {
        cout << Color::yellow << Color::bold << " (Y/N)" << Color::reset;
        cin >> answer;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (answer == 'Y' || answer == 'y') {
                clearScreen();
                return true;
        }
        if (answer == 'N' || answer == 'n') {
                clearScreen();
                return false;
        }
        cout << Color::red << Color::bold << "Lua chon khong hop le. Vui long nhap Y hoac N.\n" << Color::reset;
    }
}

void showAdminMenu() {
    cout << Color::red << Color::bold
         << " ******************************\n"
         << " *          MENU ADMIN        *\n"
         << " ******************************\n"
         << "1. Them Employee\n"
         << "2. Xoa Employee\n"
         << "3. Tim Employee\n"
         << "4. Cap nhat Employee\n"
         << "5. Hien thi tat ca Employee\n"
         << "6. Dang xuat / Thoat\n"
         << "**********************\n";
}

void adminMenu() {
    bool running = true;
    while (running) {
        clearScreen();
        showAdminMenu();
        cout << Color::red << "Chon chuc nang: ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            clearScreen();
            cout << Color::red << "Vui long nhap so tu 1 den 6.\n" << Color::reset;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();
        switch (choice) {
            case 1: case 2: case 3: case 4: case 5:
                cout << Color::red << Color::bold
                     << "Toi co phai nguoi dep trai nhat khong?"
                     << Color::reset;
                break;
            case 6:
                running = false;
                cout << Color::green << "Dang roi menu Admin...\n" << Color::reset;
                break;
            default:
                cout << Color::red << "Lua chon khong hop le!\n" << Color::reset;
        }
        if (running && !askContinue()) running = false;
    }
}

void employeeMenu(Employee& employee, const string& username) {
    bool running = true;
    while (running) {
        clearScreen();
        cout << Color::green << Color::bold
             << " ******************************\n"
             << " *        MENU EMPLOYEE       *\n"
             << " ******************************\n"
             << "1. Xem thong tin tai khoan\n"
             << "2. Doi password\n"
             << "3. Dang xuat / Thoat\n"
             << "**********************\n"
             << "Chon chuc nang: ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            clearScreen();
            cout << Color::green << "Vui long nhap so tu 1 den 3.\n" << Color::reset;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();
        switch (choice) {
            case 1:
                //employee.displayProfile(username);
                cout << Color::blue << Color::bold << "Phan nay la Profile \n" << "Return";
                break;
            case 2: {
                /*string current = readPassword("Nhap mat khau hien tai: ");
                string next = readPassword("Nhap mat khau moi: ");
                string confirm = readPassword("Nhap lai mat khau moi: ");
                if (next != confirm) {
                    cout << Color::red << "Mat khau xac nhan khong trung khop.\n" << Color::reset;
                } else if (employee.changePassword(username, current, next)) {
                    cout << Color::green << "Doi mat khau thanh cong!\n" << Color::reset;
                } else {
                    cout << Color::red << "Doi mat khau that bai. Hay kiem tra mat khau hien tai/file du lieu.\n" << Color::reset;
                }*/
                cout << Color::blue << Color::bold << "Phan nay la Doi mat khau \n" << "Return" ;
                break;
            }
            case 3:
                running = false;
                cout << Color::green << "Dang roi menu Employee...\n" << Color::reset;
                break;
            default:
                cout << Color::red << "Lua chon khong hop le!\n" << Color::reset;
        }
        if (running && !askContinue()) running = false;
    }
}

int main() {
    Employee employee("Employees.txt");
    EmployeeManager manager("Administrators.txt");
    bool programRunning = true;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    while (programRunning) {
        cout << Color::yellow << Color::bold
             << " ******************************\n"
             << " *  HE THONG QUAN LY NHAN SU  *\n"
             << " ******************************\n"
             << "1. Dang nhap Admin\n"
             << "2. Dang nhap Employee\n"
             << "3. Thoat chuong trinh\n"
             << "Lua chon: ";
        int roleChoice;
        if (!(cin >> roleChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.clear();
            clearScreen();
            cout << Color::red << "Lua chon khong hop le.\n" << Color::reset;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (roleChoice == 3) {
            programRunning = false;
            clearScreen();
            continue;
        }
        if (roleChoice != 1 && roleChoice != 2) {
            clearScreen();
            cout << Color::red << "Vui long chon 1, 2 hoac 3.\n" << Color::reset;
            continue;
        }
        clearScreen();
        cout << (roleChoice == 1 ? Color::red : Color::green)
             << "*** DANG NHAP " << (roleChoice == 1 ? "ADMIN" : "EMPLOYEE") << " ***\n"
             << "Username: ";
        string username;
        getline(cin, username);
        string password = readPassword("Password: ");

        if (roleChoice == 1) {
            if (manager.authenticate(username, password)) {
                clearScreen();
                cout << Color::green << "Dang nhap Admin thanh cong!\n" << Color::reset;
                adminMenu();
            } else {
                cout << Color::red << "Sai username/password Admin.\n" << Color::reset;
            }
        } else {
            bool loggedIn = false;
            for (int attempts = 1; attempts <= 3; ++attempts) {
                if (employee.authenticate(username, password)) {
                    loggedIn = true;
                    break;
                }
                cout << Color::red << Color::bold << "Sai username/password. Con " << (3 - attempts)
                     << " lan thu.\n" << Color::reset;
                if (attempts < 3) {
                    cout << "Username (nhap lai): ";
                    getline(cin, username);
                    password = readPassword("Password: ");
                    clearScreen();
                }
            }
            if (!loggedIn) {
                cout << Color::red << Color::bold << "Ban da dang nhap sai 3 lan. Ket thuc chuong trinh.\n" << Color::reset;
                return 0;
            }
            clearScreen();
            cout << Color::green << Color::bold << "Dang nhap Employee thanh cong!\n" << Color::reset;
            /*if (employee.needsFirstLoginPasswordChange(username)) {
                cout << Color::yellow << "Tai khoan dang dung mat khau mac dinh 111111. Vui long doi mat khau ngay.\n" << Color::reset;
                bool changed = false;
                while (!changed) {
                    string next = readPassword("Nhap mat khau moi: ");
                    string confirm = readPassword("Nhap lai mat khau moi: ");
                    if (next != confirm || next == "111111" || next.empty()) {
                        clearScreen();
                        cout << Color::red << "Mat khau khong hop le hoac khong trung khop. Thu lai.\n" << Color::reset;
                        continue;
                    }
                    changed = employee.changePassword(username, "111111", next);
                    clearScreen();
                    cout << (changed ? Color::green : Color::red)
                         << (changed ? "Doi mat khau thanh cong!\n" : "Khong the cap nhat file. Thu lai.\n")
                         << Color::reset;
                }
            }*/
            cout << Color::green << "Tai khoan dang dung mat khau mac dinh 111111. Vui long doi mat khau ngay.\n";
            cout << "Phan nay viet them bat buoc doi mat khau mac dinh 111111\n";
            pauseScreen();
            employeeMenu(employee, username);
        }
        cout << Color::red << "Roi khoi chuong trinh? \n";
        if (askContinue()) programRunning = false;
        clearScreen();
    }

    cout << Color::green << Color::bold << "Cam on ban da su dung chuong trinh!\n" << Color::reset;
    return 0;
}
