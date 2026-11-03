#include "EmployeeManager.h"
#include "Employee.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <streambuf>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

/*************************************************************
 * @Description Tu dong can giua cac dong duoc in bang std::cout.
 *              Ma mau ANSI khong duoc tinh vao do dai van ban.
 *              Van hien thi ngay cac loi nhac nhap va mat khau.
 *************************************************************/
class CenteredStreamBuf : public std::streambuf {
private:
    std::streambuf* originalBuffer_;
    std::string pendingText_;
    bool isNewLine_ = true;
    
    // Lưu lại padding của dòng tiêu đề ngay trước đó
    inline static std::size_t lastPadding_ = 0; 

    static int getConsoleWidth() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOut, &info)) {
            return info.srWindow.Right - info.srWindow.Left + 1;
        }
#else
        struct winsize size{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_col > 0) {
            return size.ws_col;
        }
#endif
        return 80;
    }

    static std::size_t getVisibleLength(const std::string& text) {
        std::size_t length = 0;
        for (std::size_t i = 0; i < text.size();) {
            unsigned char ch = static_cast<unsigned char>(text[i]);
            if (ch == 27 && i + 1 < text.size() && text[i + 1] == '[') {
                i += 2;
                while (i < text.size()) {
                    unsigned char code = static_cast<unsigned char>(text[i++]);
                    if (code >= 0x40 && code <= 0x7e) {
                        break;
                    }
                }
            } else if (ch == '\b' || ch == '\r') {
                ++i;
            } else if (ch == '\t') {
                length += 4;
                ++i;
            } else {
                if ((ch & 0xc0) != 0x80) {
                    ++length;
                }
                ++i;
            }
        }
        return length;
    }

    // HÀM KIỂM TRA TỪ KHÓA ĐẶC BIỆT
    bool isInputLine(const std::string& text) {
        static const std::vector<std::string> keywords = {
            "Username", "Password", "ho ten", "dia chi", "so dien thoai", "email",
            "Ho ten", "Dia chi", "So dien thoai", "Email"
        };
        for (const auto& keyword : keywords) {
            if (text.find(keyword) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    void writePending(bool hasNewLine) {
        bool shouldCenter = isNewLine_;
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOut, &info)) {
            shouldCenter = (info.dwCursorPosition.X == 0);
        }
#endif
        if (shouldCenter && !pendingText_.empty()) {
            int width = getConsoleWidth();
            std::size_t length = getVisibleLength(pendingText_);
            if (width > 0 && length > 0 && length < static_cast<std::size_t>(width)) {
                
                std::size_t padding = 0;

                if (isInputLine(pendingText_) && lastPadding_ > 0) {
                    padding = lastPadding_;
                }else {
                    padding = (static_cast<std::size_t>(width) - length) / 2;
                    lastPadding_ = padding; // Cập nhật lại mốc lề mới
                }

                std::string spaces(padding, ' ');
                originalBuffer_->sputn(spaces.data(), static_cast<std::streamsize>(spaces.size()));
            }
        }
        if (!pendingText_.empty()) {
            originalBuffer_->sputn(pendingText_.data(), static_cast<std::streamsize>(pendingText_.size()));
            pendingText_.clear();
        }
        if (hasNewLine) {
            originalBuffer_->sputc('\n');
        }
        isNewLine_ = hasNewLine;
    }

protected:
    int_type overflow(int_type ch) override {
        if (traits_type::eq_int_type(ch, traits_type::eof())) {
            return traits_type::not_eof(ch);
        }
        char character = traits_type::to_char_type(ch);
        if (character == '\n') {
            writePending(true);
        } else {
            pendingText_.push_back(character);
        }
        return ch;
    }

    std::streamsize xsputn(const char* text, std::streamsize count) override {
        for (std::streamsize i = 0; i < count; ++i) {
            if (traits_type::eq_int_type(overflow(traits_type::to_int_type(text[i])), traits_type::eof())) {
                return i;
            }
        }
        return count;
    }

    int sync() override {
        if (!pendingText_.empty()) {
            writePending(false);
        }
        return originalBuffer_->pubsync();
    }

public:
    explicit CenteredStreamBuf(std::streambuf* originalBuffer)
        : originalBuffer_(originalBuffer) {}

    ~CenteredStreamBuf() override {
        sync();
    }
};
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
    int ch = std::getchar();
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
    std::system("cls");
#else
    std::system("clear");
#endif
}

namespace Color {
    const std::string reset = "\033[0m";
    const std::string red = "\033[31m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";
    const std::string blue = "\033[34m";
    const std::string cyan = "\033[36m";
    const std::string bold = "\033[1m";
}

/*************************************************************
 * @Description Đọc chuỗi mật khẩu ẩn từ bàn phím và hiển thị ký tự '*' thay thế
 *
 * @return      Chuỗi mật khẩu người dùng đã nhập
 * @attention   Hỗ trợ xử lý phím Backspace để xóa ký tự khi nhập sai
 *************************************************************/
std::string readPassword(const std::string& strPrompt) {
    std::cout << strPrompt << std::flush;
    std::string strPassword;
    while (true) {
        int iCh = readHiddenChar();
        if (iCh == '\r' || iCh == '\n') {
            std::cout << '\n';
            break;
        }
        if (iCh == 8) { // Xử lý Backspace
            if (!strPassword.empty()) {
                strPassword.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        } else if (iCh == 0 || iCh == 224) {
            (void)readHiddenChar();
        } else if (iCh >= 32 && iCh <= 126) {
            strPassword.push_back(static_cast<char>(iCh));
            std::cout << '*' << std::flush; // In dấu sao che mật khẩu
        }
    }
    return strPassword;
}

/*************************************************************
 * @Description Dừng màn hình và chờ người dùng nhấn Enter để tiếp tục
 *************************************************************/
void pauseScreen() {
    std::cout << Color::yellow << "Nhan Enter de tiep tuc..." << Color::reset;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
        std::cout << Color::yellow << Color::bold << "(Y/N): " << Color::reset;
        std::cin >> cAnswer;
        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (cAnswer == 'Y' || cAnswer == 'y') {
            clearScreen();
            return true;
        }
        if (cAnswer == 'N' || cAnswer == 'n') {
            clearScreen();
            return false;
        }
        std::cout << Color::red << Color::bold << "Lua chon khong hop le. Vui long nhap Y hoac N.\n" << Color::reset;
    }
}

/*************************************************************
 * @Description Hiển thị giao diện menu dành cho tài khoản Admin
 *************************************************************/
void showAdminMenu() {
    std::cout << Color::red << Color::bold
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
        std::cout << Color::red << "Chon chuc nang: ";
        int iChoice;
        if (!(std::cin >> iChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            clearScreen();
            std::cout << Color::red << "Vui long nhap so tu 1 den 6.\n" << Color::reset;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        clearScreen();
        // Tìm employee theo username (chức năng 3) và hiển thị toàn bộ employee (chức năng 5)
        switch (iChoice) {
            // Nhập thông tin nhân viên mới và gọi hàm addEmployee()
            case 1: {
                std::string strUsername, strFullName, strAddress, strPhone, strEmail;
                std::cout << "Username: ";
                std::getline(std::cin, strUsername);
                std::cout << "Ho ten: ";
                std::getline(std::cin, strFullName);
                std::cout << "Dia chi: ";
                std::getline(std::cin, strAddress);
                std::cout << "So dien thoai: ";
                std::getline(std::cin, strPhone);
                std::cout << "Email: ";
                std::getline(std::cin, strEmail);

                if (manager.addEmployee(strUsername, strFullName, strAddress, strPhone, strEmail)) {
                    std::cout << "Them nhan vien thanh cong. Password mac dinh: 111111\n";
                } else {
                    std::cout << "Them nhan vien khong thanh cong.\n";
                }
                break;
            }
            // Xác nhận yêu cầu và gọi hàm removeEmployee()
            case 2: {
                std::string strUsername;
                std::cout << "Username can xoa: ";
                std::getline(std::cin, strUsername);
                char cConfirm;
                std::cout << "Ban co chac muon xoa? (y/n): "; std::cin >> cConfirm;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (cConfirm != 'y' && cConfirm != 'Y') {
                    std::cout << "Da huy xoa.\n";
                    break;
                }
                if (manager.removeEmployee(strUsername)) {
                    std::cout << "Xoa nhan vien thanh cong.\n";
                } else {
                    std::cout << "Xoa nhan vien khong thanh cong.\n";
                }
                break;
            }
            case 3: {
                std::string strFindUsername;
                std::cout << Color::reset << "Username can tim: ";
                std::getline(std::cin, strFindUsername);
                manager.searchEmployee(strFindUsername);
                break;
            }
            case 4: {
                std::string strUsername;
                std::cout << "Username can cap nhat: ";
                std::getline(std::cin, strUsername);
                manager.updateEmployee(strUsername);
                break;
            }
            case 5:
                manager.displayAllEmployees();
                break;
            case 6:
                bRunning = false;
                std::cout << Color::green << "Dang roi menu Admin...\n" << Color::reset;
                break;
            default:
                std::cout << Color::red << "Lua chon khong hop le!\n" << Color::reset;
        }

        if (bRunning) {
            std::cout << "Return? ";
            bRunning = askContinue();
        }
    }
}

/*************************************************************
 * @Description Quản lý các chức năng trong menu Employee
 *
 * @attention   Cho phép xem thông tin profile cá nhân, đổi mật khẩu tài khoản và đăng xuất  quyền nhân viên
 *************************************************************/
void employeeMenu(Employee& employee, const std::string& strUsername) {
    bool bRunning = true;
    while (bRunning) {
        clearScreen();
        std::cout << Color::green << Color::bold
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
        if (!(std::cin >> iChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            clearScreen();
            std::cout << Color::green << "Vui long nhap so tu 1 den 3.\n" << Color::reset;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        clearScreen();
         // Employee xem thông tin tài khoản của chính mình (chức năng 1)
        switch (iChoice) {
            case 1: {
                EmployeeManager profileManager;
                profileManager.viewOwnProfile(strUsername);
                break;
            }

            case 2: {
                std::string strCurrent = readPassword("Nhap mat khau hien tai: ");
                std::string strNext = readPassword("Nhap mat khau moi: ");
                std::string strConfirm = readPassword("Nhap lai mat khau moi: ");
                if (strNext != strConfirm) {
                    std::cout << Color::red << "Mat khau xac nhan khong trung khop.\n" << Color::reset;
                } else if (employee.changePassword(strUsername, strCurrent, strNext)) {
                    std::cout << Color::green << "Doi mat khau thanh cong!\n" << Color::reset;
                } else {
                    std::cout << Color::red << "Doi mat khau that bai. Hay kiem tra lai mat khau hien tai.\n" << Color::reset;
                }
                break;
            }
            case 3:
                bRunning = false;
                std::cout << Color::green << "Dang roi menu Employee...\n" << Color::reset;
                break;
            default:
                std::cout << Color::red << "Lua chon khong hop le!\n" << Color::reset;
        }
        if (bRunning) {
            std::cout << "Return? ";
            bRunning = askContinue();
        }
    }
}

int main() {
    CenteredStreamBuf centeredOutput(std::cout.rdbuf());
    std::streambuf* originalOutput = std::cout.rdbuf(&centeredOutput);

    Employee employee("Employees.txt");
    EmployeeManager manager("Administrators.txt");
    bool bProgramRunning = true;

#ifdef _WIN32
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
#endif

    while (bProgramRunning) {
        clearScreen();
        std::cout << Color::yellow << Color::bold
             << " ******************************\n"
             << " *  HE THONG QUAN LY NHAN SU  *\n"
             << " ******************************\n"
             << Color::reset
             << "1. Dang nhap Admin\n"
             << "2. Dang nhap Employee\n"
             << "3. Thoat chuong trinh\n"
             << "Lua chon: ";
        int iRoleChoice;
        if (!(std::cin >> iRoleChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            clearScreen();
            std::cout << Color::red << "Lua chon khong hop le.\n" << Color::reset;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (iRoleChoice == 3) {
            bProgramRunning = false;
            clearScreen();
            break;
        }
        if (iRoleChoice != 1 && iRoleChoice != 2) {
            clearScreen();
            std::cout << Color::red << "Vui long chon 1, 2 hoac 3.\n" << Color::reset;
            pauseScreen();
            continue;
        }

        clearScreen();
        std::cout << (iRoleChoice == 1 ? Color::red : Color::green)
             << "******** DANG NHAP " << (iRoleChoice == 1 ? "ADMIN" : "EMPLOYEE") << " ********\n"
             << Color::reset << "Username: ";
        std::string strUsername;
        std::getline(std::cin, strUsername);
        std::string strPassword = readPassword("Password: ");
        clearScreen();

        if (iRoleChoice == 1) {
            if (manager.authenticateManager(strUsername, strPassword)) {
                std::cout << Color::green << "Dang nhap Admin thanh cong!\n" << Color::reset;
                pauseScreen();
                adminMenu(manager);
            } else {
                std::cout << Color::red << "Sai username/password Admin.\n" << Color::reset;
            }
        } else {
            bool bLoggedIn = false;
            // Cho phép người dùng đăng nhập tối đa 3 lần
            for (int iAttempts = 1; iAttempts <= 3; ++iAttempts) {
                if (employee.authenticateEmployee(strUsername, strPassword)) {
                    bLoggedIn = true;
                    break;
                }
                std::cout << Color::red << Color::bold << "Sai username/password. Con " << (3 - iAttempts)
                     << " lan thu.\n" << Color::reset;
                if (iAttempts < 3) {
                    std::cout << "Username (nhap lai): ";
                    std::getline(std::cin, strUsername);
                    strPassword = readPassword("Password: ");
                    clearScreen();
                }
            }
            if (!bLoggedIn) {
                std::cout << Color::red << Color::bold << "Ban da dang nhap sai 3 lan. Ket thuc chuong trinh.\n" << Color::reset;
                pauseScreen();
                break;
            }

            std::cout << Color::green << Color::bold << "Dang nhap Employee thanh cong!\n" << Color::reset;
            // Kiểm tra nếu nhân viên đăng nhập lần đầu với mật khẩu mặc định "111111"
            if (employee.needsFirstLoginPasswordChange(strUsername)) {
                std::cout << Color::yellow << "Tai khoan dang dung mat khau mac dinh 111111. Vui long doi mat khau ngay.\n" << Color::reset;
                bool bChanged = false;
                while (!bChanged) {
                    std::string strNext = readPassword("Nhap mat khau moi: ");
                    std::string strConfirm = readPassword("Nhap lai mat khau moi: ");
                    if (strNext != strConfirm || strNext == "111111" || strNext.empty()) {
                        clearScreen();
                        std::cout << Color::red << "Mat khau khong hop le hoac khong trung khop. Thu lai.\n" << Color::reset;
                        continue;
                    }
                    bChanged = employee.changePassword(strUsername, "111111", strNext);
                    clearScreen();
                    std::cout << (bChanged ? Color::green : Color::red)
                         << (bChanged ? "Doi mat khau thanh cong!\n" : "Khong the cap nhat file. Thu lai.\n")
                         << Color::reset;
                }
            }
            pauseScreen();
            employeeMenu(employee, strUsername);
        }

        if (bProgramRunning) {
            std::cout << "Thoat khoi chuong trinh? ";
            bProgramRunning = !askContinue();
        }
    }

    clearScreen();
    std::cout << Color::green << Color::bold << "Cam on ban da su dung chuong trinh!\n" << Color::reset;
    std::cout.flush();
    std::cout.rdbuf(originalOutput);
    return 0;
}
