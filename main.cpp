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
    system("cls");
#else
    system("clear");
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

string readPassword(const string& strPrompt) {
    cout << strPrompt;
    string strPassword;
    while (true) {
        int iCh = readHiddenChar();
        if (iCh == '\r' || iCh == '\n') {
            cout << '\n';
            break;
        }
        if (iCh == 8) { // Backspace
            if (!strPassword.empty()) {
                strPassword.pop_back();
                cout << "\b \b";
            }
        } else if (iCh == 0 || iCh == 224) {
            (void)readHiddenChar();
        } else if (iCh >= 32 && iCh <= 126) {
            strPassword.push_back(static_cast<char>(iCh));
            cout << '*';
        }
    }
    return strPassword;
}

void pauseScreen() {
    cout << Color::yellow << "Nhan Enter de tiep tuc..." << Color::reset;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool askContinue() {
    char cAnswer;
    while (true) {
        cout << Color::yellow << Color::bold << "(Y/N): " << Color::reset;
        cin >> cAnswer;
        if (!cin) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (cAnswer == 'Y' || cAnswer == 'y') {
            clearScreen();
            return true;
        }
        if (cAnswer == 'N' || cAnswer == 'n') {
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
         << Color::reset
         << "1. Them Employee\n"
         << "2. Xoa Employee\n"
         << "3. Tim Employee\n"
         << "4. Cap nhat Employee\n"
         << "5. Hien thi tat ca Employee\n"
         << "6. Dang xuat / Thoat\n"
         << Color::red << "**********************\n" << Color::reset;
}

void adminMenu(EmployeeManager& manager) {
    bool bRunning = true;
    while (bRunning) {
        clearScreen();
        showAdminMenu();
        cout << Color::red << "Chon chuc nang: ";
        int iChoice;
        if (!(cin >> iChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            clearScreen();
            cout << Color::red << "Vui long nhap so tu 1 den 6.\n" << Color::reset;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();
        switch (iChoice) {
            case 1: case 2: case 3: case 4: case 5:
                cout << Color::red << Color::bold << "Chuc nang nay dang duoc phat trien boi thanh vien trong nhom.\n" << Color::reset;
                break;
            case 6:
                bRunning = false;
                cout << Color::green << "Dang roi menu Admin...\n" << Color::reset;
                break;
            default:
                cout << Color::red << "Lua chon khong hop le!\n" << Color::reset;
        }

        if (bRunning) {
            cout << "Return? ";
            bRunning = askContinue();
        }
    }
}

void employeeMenu(Employee& employee, const string& strUsername) {
    bool bRunning = true;
    while (bRunning) {
        clearScreen();
        cout << Color::green << Color::bold
             << " ******************************\n"
             << " *        MENU EMPLOYEE       *\n"
             << " ******************************\n"
             << Color::reset
             << "1. Xem thong tin tai khoan\n"
             << "2. Doi password\n"
             << "3. Dang xuat / Thoat\n"
             << Color::green << "**********************\n" << Color::reset
             << "Chon chuc nang: ";
        int iChoice;
        if (!(cin >> iChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            clearScreen();
            cout << Color::green << "Vui long nhap so tu 1 den 3.\n" << Color::reset;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();
        switch (iChoice) {
            case 1:
                cout << Color::red << Color::bold << "Chuc nang nay dang duoc phat trien boi thanh vien trong nhom.\n" << Color::reset;  //TODO: Thêm tính năng hiển thị thông tin cá nhân của Employee
                break;
            case 2:
                cout << Color::red << Color::bold << "Chuc nang nay dang duoc phat trien boi thanh vien trong nhom.\n" << Color::reset;  //TODO: Thêm tính năng đổi mật khẩu của Employee
                break;
            case 3:
                bRunning = false;
                cout << Color::green << "Dang roi menu Employee...\n" << Color::reset;
                break;
            default:
                cout << Color::red << "Lua chon khong hop le!\n" << Color::reset;
        }
        if (bRunning) {
            cout << "Return? ";
            bRunning = askContinue();
        }
    }
}

int main() {
    Employee employee("Employees.txt");
    EmployeeManager manager("Administrators.txt");
    bool bProgramRunning = true;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    while (bProgramRunning) {
        clearScreen();
        cout << Color::yellow << Color::bold
             << " ******************************\n"
             << " *  HE THONG QUAN LY NHAN SU  *\n"
             << " ******************************\n"
             << Color::reset
             << "1. Dang nhap Admin\n"
             << "2. Dang nhap Employee\n"
             << "3. Thoat chuong trinh\n"
             << "Lua chon: ";
        int iRoleChoice;
        if (!(cin >> iRoleChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            clearScreen();
            cout << Color::red << "Lua chon khong hop le.\n" << Color::reset;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (iRoleChoice == 3) {
            bProgramRunning = false;
            clearScreen();
            break;
        }
        if (iRoleChoice != 1 && iRoleChoice != 2) {
            clearScreen();
            cout << Color::red << "Vui long chon 1, 2 hoac 3.\n" << Color::reset;
            pauseScreen();
            continue;
        }

        clearScreen();
        cout << (iRoleChoice == 1 ? Color::red : Color::green)
             << "*** DANG NHAP " << (iRoleChoice == 1 ? "ADMIN" : "EMPLOYEE") << " ***\n"
             << Color::reset << "Username: ";
        string strUsername;
        getline(cin, strUsername);
        string strPassword = readPassword("Password: ");
        clearScreen();

        if (iRoleChoice == 1) {
            if (manager.authenticateManager(strUsername, strPassword)) {
                cout << Color::green << "Dang nhap Admin thanh cong!\n" << Color::reset;
                pauseScreen();
                adminMenu(manager);
            } else {
                cout << Color::red << "Sai username/password Admin.\n" << Color::reset;
            }
        } else {
            bool bLoggedIn = false;
            for (int iAttempts = 1; iAttempts <= 3; ++iAttempts) {
                if (employee.authenticateEmployee(strUsername, strPassword)) {
                    bLoggedIn = true;
                    break;
                }
                cout << Color::red << Color::bold << "Sai username/password. Con " << (3 - iAttempts)
                     << " lan thu.\n" << Color::reset;
                if (iAttempts < 3) {
                    cout << "Username (nhap lai): ";
                    getline(cin, strUsername);
                    strPassword = readPassword("Password: ");
                    clearScreen();
                }
            }
            if (!bLoggedIn) {
                cout << Color::red << Color::bold << "Ban da dang nhap sai 3 lan. Ket thuc chuong trinh.\n" << Color::reset;
                break;
            }

            cout << Color::green << Color::bold << "Dang nhap Employee thanh cong!\n" << Color::reset;
            cout << Color::yellow << "Tai khoan dang dung mat khau mac dinh 111111. Vui long doi mat khau ngay.\n" << Color::reset;  //TODO: Thêm phần bắt buộc đổi mật khẩu khi Employee đăng nhập lần đầu
            pauseScreen();
            employeeMenu(employee, strUsername);
        }

        if (bProgramRunning) {
            cout << "Thoat khoi chuong trinh? ";
            bProgramRunning = !askContinue();
        }
    }

    clearScreen();
    cout << Color::green << Color::bold << "Cam on ban da su dung chuong trinh!\n" << Color::reset;
    return 0;
}
