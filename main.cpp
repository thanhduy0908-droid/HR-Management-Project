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

/*************************************************************
 * @Description Đọc một ký tự từ bàn phím mà không hiển thị lên màn hình (ẩn password)
 *
 * @return      Mã ASCII của ký tự được nhập từ bàn phím
 * @attention   Hỗ trợ tương thích đa nền tảng (_WIN32 dùng _getch, Linux/Mac dùng termios)
 *************************************************************/
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

/*************************************************************
 * @Description Xóa sạch màn hình Terminal hiện tại
 *
 * @attention   Sử dụng lệnh "cls" trên Windows và "clear" trên hệ thống Unix/Linux
 *************************************************************/
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

/*************************************************************
 * @Description Đọc chuỗi mật khẩu ẩn từ bàn phím và hiển thị ký tự '*' thay thế
 *
 * @return      Chuỗi mật khẩu người dùng đã nhập
 * @attention   Hỗ trợ xử lý phím Backspace để xóa ký tự khi nhập sai
 *************************************************************/
string readPassword(const string& strPrompt) {
    cout << strPrompt;
    string strPassword;
    while (true) {
        int iCh = readHiddenChar();
        if (iCh == '\r' || iCh == '\n') {
            cout << '\n';
            break;
        }
        if (iCh == 8) { // Xử lý Backspace
            if (!strPassword.empty()) {
                strPassword.pop_back();
                cout << "\b \b";
            }
        } else if (iCh == 0 || iCh == 224) {
            (void)readHiddenChar();
        } else if (iCh >= 32 && iCh <= 126) {
            strPassword.push_back(static_cast<char>(iCh));
            cout << '*'; // In dấu sao che mật khẩu
        }
    }
    return strPassword;
}

/*************************************************************
 * @Description Dừng màn hình và chờ người dùng nhấn Enter để tiếp tục
 *************************************************************/
void pauseScreen() {
    cout << Color::yellow << "Nhan Enter de tiep tuc..." << Color::reset;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/*************************************************************
 * @Description Hỏi người dùng có muốn tiếp tục tác vụ được gọi tên hay không (Y/N)
 *
 * @return      true nếu chọn Y/y, false nếu chọn N/n
 * @attention   Xử lý kiểm tra lỗi nhập sai ký tự và yêu cầu nhập lại
 *************************************************************/
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

/*************************************************************
 * @Description Hiển thị giao diện menu dành cho tài khoản Admin
 *************************************************************/
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

/*************************************************************
 * @Description Quản lý các chức năng trong menu Admin
 *
 * @attention   Cho phép thêm, xóa, tìm, cập nhật, hiển thị tất cả nhân viên và đăng xuất quyền admin
 *************************************************************/
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
            // Nhập thông tin nhân viên mới và gọi hàm addEmployee()
            case 1: {
                string strUsername, strFullName, strAddress, strPhone, strEmail;
                cout << "Username: ";
                getline(cin, strUsername);
                cout << "Ho ten: ";
                getline(cin, strFullName);
                cout << "Dia chi: ";
                getline(cin, strAddress);
                cout << "So dien thoai: ";
                getline(cin, strPhone);
                cout << "Email: ";
                getline(cin, strEmail);
            
                if (manager.addEmployee(strUsername, strFullName, strAddress, strPhone, strEmail)) {
                    cout << "Them nhan vien thanh cong. Password mac dinh: 111111\n";
                } else {
                    cout << "Them nhan vien khong thanh cong.\n";
                }
                break;
            }
             // Xác nhận yêu cầu và gọi hàm removeEmployee()
            case 2: {
                string strUsername;
                cout << "Username can xoa: ";
                getline(cin, strUsername);
                char cConfirm;
                cout << "Ban co chac muon xoa? (y/n): "; cin >> cConfirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (cConfirm != 'y' && cConfirm != 'Y') {
                    cout << "Da huy xoa.\n";
                    break;
                }
                if (manager.removeEmployee(strUsername)) {
                    cout << "Xoa nhan vien thanh cong.\n";
                } else {
                    cout << "Xoa nhan vien khong thanh cong.\n";
                }
                break;
            }
            // Tìm employee theo username (chức năng 3)
            case 3: {
                string strFindUsername;
                cout << Color::reset << "Username can tim: ";
                getline(cin, strFindUsername);
                manager.searchEmployee(strFindUsername);
                break;
            }
             case 4: {
                string strUsername;
                cout << "Username can cap nhat: ";
                getline(cin, strUsername);
                manager.updateEmployee(strUsername);
                break;
            }
            // Hiển thị toàn bộ employee (chức năng 5)
            case 5:
                manager.displayAllEmployees();
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

/*************************************************************
 * @Description Quản lý các chức năng trong menu Employee
 *
 * @attention   Cho phép xem thông tin profile cá nhân, đổi mật khẩu tài khoản và đăng xuất  quyền nhân viên
 *************************************************************/
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
            // Employee xem thông tin tài khoản của chính mình (chức năng 1)
            case 1: {
                EmployeeManager profileManager;
                profileManager.viewOwnProfile(strUsername);
                break;
            } 
            case 2:
                cout << Color::red << Color::bold << "Chuc nang nay dang duoc phat trien boi thanh vien trong nhom.\n" << Color::reset;
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

    // Cấu hình hỗ trợ mã màu ANSI Escape Sequences trên terminal của Windows
    // Lấy định danh (handle) của màn hình console đầu ra chuẩn (Standard Output)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    // Kiểm tra xem handle có hợp lệ không, đồng thời lấy các chế độ console hiện tại
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        // Bật cờ hỗ trợ xử lý mã điều khiển Terminal ảo (cho phép in chữ có màu sắc, đổi màu chữ,...)
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        // Cập nhật lại chế độ mới cho console của Windows
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
            // Cho phép người dùng đăng nhập tối đa 3 lần
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
                pauseScreen();
                break;
            }

            cout << Color::green << Color::bold << "Dang nhap Employee thanh cong!\n" << Color::reset;
            cout << Color::yellow << "Tai khoan dang dung mat khau mac dinh 111111. Vui long doi mat khau ngay.\n" << Color::reset;
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
